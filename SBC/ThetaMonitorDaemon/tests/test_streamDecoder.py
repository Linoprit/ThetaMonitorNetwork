import time
import unittest
from queue import Queue

from serialIO.streamDecoder import StreamDecoder as StreamDecoder
from tests.unittest_setup import Unittest_setup


# noinspection SpellCheckingInspection
class TestStreamDecoder(unittest.TestCase):
    setup = Unittest_setup()
    settings = setup.get_standard_settings()

    def test_receive_text_msg(self):
        text_queue = Queue()
        bin_queue = Queue()
        sd = StreamDecoder(text_queue, bin_queue)
        tst_string = 'Message from hell.\n'
        in_buffer = bytes(tst_string, "utf8")
        sd.proc_serial_stream(in_buffer)
        self.assertEqual(19, text_queue.qsize())
        self.assertEqual(tst_string, sd.get_txt_queue_as_string(text_queue))

        text_queue.task_done()
        time.sleep(0.4)  # Just enough to let the Queue finish

    def test_receive_bin_msg(self):
        bin_msg = [17, 2, 18, 0, 1, 0, 0, 0, 197, 242, 158,
                   212, 0, 0, 147, 65, 162, 88, 9, 0, 162, 3, 19]
        text_queue = Queue()
        bin_queue = Queue()
        sd = StreamDecoder(text_queue, bin_queue)
        sd.proc_serial_stream(bytes(bin_msg))
        self.assertEqual(1, bin_queue.qsize())
        result = bin_queue.get()

        self.assertEqual(162, result.checksum)
        self.assertEqual(612514, result.lastUpdateTick)
        self.assertEqual(0, result.msgClass)
        self.assertEqual(3567186629, result.sensorIdHash)
        self.assertEqual(1, result.stationId)
        self.assertEqual(18.375, result.value)

        bin_queue.task_done()
        time.sleep(0.4)  # Just enough to let the Queue finish

    @staticmethod
    def put_bytes_into_array(target: list, source):
        for i in range(0, len(source)):
            target.append(source[i])

    def test_receive_mixed_msg(self):
        bin_msg_01 = [17, 2, 18, 0, 1, 0, 0, 0, 197, 242, 158,
                      212, 0, 0, 147, 65, 162, 88, 9, 0, 162, 3, 19]
        bin_msg_02 = [17, 2, 18, 0, 1, 0, 0, 0, 135, 1, 212,
                      227, 0, 128, 152, 65, 200, 139, 9, 0, 244, 3, 19]
        txt_msg_01 = "first part of msg...\n"
        txt_msg_02 = "second part, sent by someone.\n"

        bin_msg = []
        self.put_bytes_into_array(bin_msg, bin_msg_01)
        self.put_bytes_into_array(bin_msg, bytes(txt_msg_01, "utf8"))
        self.put_bytes_into_array(bin_msg, bin_msg_02)
        self.put_bytes_into_array(bin_msg, bytes(txt_msg_02, "utf8"))

        text_queue = Queue()
        bin_queue = Queue()
        sd = StreamDecoder(text_queue, bin_queue)
        sd.proc_serial_stream(bytes(bin_msg))

        self.assertEqual(2, bin_queue.qsize())
        self.assertEqual(51, text_queue.qsize())

        result = bin_queue.get()
        self.assertEqual(162, result.checksum)
        self.assertEqual(612514, result.lastUpdateTick)
        self.assertEqual(0, result.msgClass)
        self.assertEqual(3567186629, result.sensorIdHash)
        self.assertEqual(1, result.stationId)
        self.assertEqual(18.375, result.value)

        result = bin_queue.get()
        self.assertEqual(244, result.checksum)
        self.assertEqual(625608, result.lastUpdateTick)
        self.assertEqual(0, result.msgClass)
        self.assertEqual(3822322055, result.sensorIdHash)
        self.assertEqual(1, result.stationId)
        self.assertEqual(19.0625, result.value)

        text = sd.get_txt_queue_as_string(text_queue)
        self.assertEqual(txt_msg_01 + txt_msg_02, text)

        bin_queue.task_done()
        text_queue.task_done()
        time.sleep(0.4)  # Just enough to let the Queue finish

    # Here, the c-types for documentation:
    # 	typedef struct PACKED {
    # 		MsgClass msgClass :8;
    # 		uint32_t stationId;
    # 		uint8_t payload[nRF_PAYLOAD_LEN];
    # 		uint8_t checksum;
    # 	} RadioMessageType; //  6 bytes + payload
    #
    # enum MsgClass { MEASUREMENT, STATISTICS, COMMAND, REQUEST };
    #
    # typedef struct PACKED {
    # 	uint32_t stationId; // is redundant
    # 	uint8_t relayStates; // bitfield
    # 	uint8_t lostPkgs;
    # 	uint8_t validSensors;
    # 	uint8_t rxBufferOverflows;
    # 	uint32_t lastUpdateTick; // TODO should become a timestamp later!
    # } RadioStatisticsType;  // 12 bytes
    #
    #  typedef struct PACKED {
    # 	uint32_t sensorIdHash;
    # 	float value;
    # 	uint32_t lastUpdateTick; // TODO should become a timestamp later!
    # } MeasurementType;  // 12 bytes
