import logging
import math
from enum import Enum, auto
from queue import Queue
from struct import *

from serialIO.calcStm32Crc import CalcStm32Crc

# -------- definitions from Stm-Code --------
# https://www.windmill.co.uk/ascii-control-codes.html
# Ctrl	ASCII 	Dec 	Hex	Meaning
# ^Q 	DCL 	017 	11 	Xon (transmit on)
# ^B 	STX		002 	02 	Start of Text
# ^C 	ETX		003 	03 	End of Text
# ^S 	DC3 	019 	13	Xoff (transmit off)
XON = 17  # Starts transmission
STX = 2  # Begin of text
ETX = 3  # End of text
XOF = 19  # Ends transmission

# from enum MsgClass(Enum)
MEASUREMENT_ENUM = 0
STATISTICS_ENUM = 1
COMMAND_ENUM = 2
REQUEST_ENUM = 3


class RadioMessageType:
    msgClass = -1
    stationId = -1
    checksum = -1

    def __init__(self, msgClass: int, stationId: int, checksum: int):
        self.msgClass = msgClass
        self.stationId = stationId
        self.checksum = checksum


class MeasurementType(RadioMessageType):
    sensorIdHash = -1
    value = 0.0
    lastUpdateTick = -1

    def __init__(self, binary_message: list):
        try:
            msgClass, stationId, sensorIdHash, \
                value, lastUpdateTick, checksum = \
                unpack(MEASUREMENT_FORMAT, bytes(binary_message))
        except ValueError as e:
            print("ValueError exception: " + str(e), end='')
            print(binary_message)
            return
        super().__init__(msgClass, stationId, checksum)
        self.sensorIdHash = sensorIdHash
        self.value = value
        self.lastUpdateTick = lastUpdateTick


class RadioStatisticsType(RadioMessageType):
    msgClass = -1
    stationId = -1
    stationId2 = -1
    relayStates = -1
    lostPkgs = -1
    validSensors = -1
    rxBufferOverflows = -1
    lastUpdateTick = -1
    checksum = -1

    def __init__(self, binary_message: list):
        msgClass, stationId, stationId2, relayStates, \
            lostPkgs, validSensors, rxBufferOverflows, \
            lastUpdateTick, checksum = \
            unpack(STATISTIC_FORMAT, bytes(binary_message))

        super().__init__(msgClass, stationId, checksum)
        self.relayStates = relayStates
        self.lostPkgs = lostPkgs
        self.validSensors = validSensors
        self.rxBufferOverflows = rxBufferOverflows
        self.lastUpdateTick = lastUpdateTick


MEASUREMENT_FORMAT = '<BLLfLB'
STATISTIC_FORMAT = '<BLLBBBBLB'


# -------- decoder state Machine --------
class StreamState(Enum):
    INIT = auto()
    STORE_TEXT = auto()
    TRM_STARTED = auto()
    RCV_BIN_MESSAGE = auto()
    AWAIT_END_TRM = auto()


class UnknownStateError(Exception):
    pass


class StreamDecoder:
    def __init__(self, text_queue: Queue, bin_queue: Queue):
        self._old_state = StreamState.INIT
        self._act_state = StreamState.INIT
        self._bin_msg_len = 0
        self._bin_tmp_buffer = []
        self._text_queue = text_queue
        self._bin_queue = bin_queue
        self._crc_fail_count = 0

    def proc_serial_stream(self, stream_data: bytes):
        for char in stream_data:
            self._cycle(char)

    def _cycle(self, char):
        if self._act_state == StreamState.INIT:
            self._init_transit(char)
        elif self._act_state == StreamState.STORE_TEXT:
            self._store_text_state(char)
        elif self._act_state == StreamState.TRM_STARTED:
            self._trm_started_state(char)
        elif self._act_state == StreamState.RCV_BIN_MESSAGE:
            self._rcv_bin_message_state(char)
        elif self._act_state == StreamState.AWAIT_END_TRM:
            self._await_end_trm_state(char)
        else:
            raise UnknownStateError("Unknown state.")

    # -------- states --------
    def _trm_started_state(self, char):
        if self._is_state_new():  # entry code
            self._new_state_entered()
        self._trm_started_transit(char)

    def _store_text_state(self, char):
        if self._is_state_new():  # entry code
            self._new_state_entered()
        result = self._store_text_transit(char)  # check transitions
        if not result:  # cycle code
            self._text_queue.put(chr(char), block=True, timeout=0.1)

    def _rcv_bin_message_state(self, char):
        if self._is_state_new():  # entry code
            self._new_state_entered()
            self._bin_msg_len = char
            self._bin_tmp_buffer.clear()
            return
        result = self._rcv_bin_message_transit()  # check transitions
        if not result:  # cycle code
            self._bin_tmp_buffer.append(char)
        else:  # exit code
            crc_ok = CalcStm32Crc().check_crc_from_serial(self._bin_tmp_buffer)
            if crc_ok:
                msmt = self.get_bin_msg_decoded(self._bin_tmp_buffer)
                if msmt is not None:
                    self._bin_queue.put(msmt, block=True, timeout=None)
            else:
                self._crc_fail_count += 1

    def _await_end_trm_state(self, char):
        if self._is_state_new():  # entry code
            self._new_state_entered()
        self._await_end_trm_transit(char)  # check transitions

    # -------- transitions --------
    def _init_transit(self, char):
        if char == XON:
            self._set_new_state(StreamState.TRM_STARTED)
        else:
            self._set_new_state(StreamState.STORE_TEXT)
            self._store_text_state(char)
        return True

    def _store_text_transit(self, char):
        if char == XON:
            self._set_new_state(StreamState.TRM_STARTED)
            return True
        return False

    def _trm_started_transit(self, char):
        if char == STX:
            self._set_new_state(StreamState.RCV_BIN_MESSAGE)
        else:
            self._set_new_state(StreamState.STORE_TEXT)
        return True

    def _rcv_bin_message_transit(self):
        if self._bin_msg_len == len(self._bin_tmp_buffer):
            self._set_new_state(StreamState.AWAIT_END_TRM)
            return True
        return False

    def _await_end_trm_transit(self, char):
        if char == XOF:
            self._set_new_state(StreamState.INIT)
        else:
            self._set_new_state(StreamState.STORE_TEXT)
        return True

    # -------- helpers --------
    @staticmethod
    def get_txt_queue_as_string(text_queue: Queue) -> str:
        result = ""
        while not text_queue.empty():
            result += text_queue.get()
        return result

    @staticmethod
    def get_bin_msg_decoded(bin_msg):
        message_class = bin_msg[0]
        if message_class == MEASUREMENT_ENUM:
            if len(bin_msg) != 18:
                return None
            result = MeasurementType(bin_msg)
            if math.isnan(result.value):
                logging.getLogger().error(
                    "Received NaN-value. SensorId: {}".format(result.sensorIdHash))
                return None
            return result
        elif message_class == STATISTICS_ENUM:
            result = RadioStatisticsType(bin_msg)
            return result
        elif message_class == COMMAND_ENUM:
            pass  # will not be sent
        elif message_class == REQUEST_ENUM:
            pass  # will not be sent
        return None

    def reset_crc_fail_count(self):
        self._crc_fail_count = 0

    def _is_state_new(self) -> bool:
        return self._old_state != self._act_state

    def _set_new_state(self, new_state: StreamState):
        self._old_state = self._act_state
        self._act_state = new_state

    def _new_state_entered(self):
        self._old_state = self._act_state
