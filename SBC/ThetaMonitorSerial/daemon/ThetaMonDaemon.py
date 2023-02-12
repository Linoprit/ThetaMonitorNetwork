import signal
import sys
import time
from queue import Queue

import framework.settings
from daemon.DataBaseConnector import DataBaseConnector as Dbcon
from serialIO.serialThread import Worker
from serialIO.streamDecoder import StreamDecoder as StreamDecoder
from datetime import timedelta, datetime


# Desiderata:
# Ungültige Sensordaten kennzeichnen und nicht mehr in die db schieben
#   (ticks checken und wenn zu alt aus liste löschen)
# html bauen





# ctrl+c signal handler
def signal_handler(sig, frame):
    print('Exit requested, waiting for all threads to exit.')
    ThetaMonDaemon.exit_requested = True


class ThetaMonDaemon:
    exit_requested = False

    def __init__(self, settings_in: framework.settings.AppSettings):
        self.settings = settings_in
        self.serial_queue = Queue()
        self.txt_queue = Queue()
        self.bin_queue = Queue()
        self.stream_decoder = StreamDecoder(self.txt_queue, self.bin_queue)
        self.sensor_val_dic = {}  # { sensorId, MeasurementType }
        self.statistic_dic = {}  # { sensorId, RadioStatisticsType }
        self.worker = None
        signal.signal(signal.SIGINT, signal_handler)
        self.db = Dbcon(self.settings)
        self.next_time = datetime.now() + timedelta(hours=0, minutes=5)
        self.entry()

    def entry(self):
        device = self.settings.get("common", "serial_port")
        self.worker = Worker(self.serial_queue, device, 115200, 0.5)
        self.worker.start()
        self.db.connect()

        while 1:
            if self.exit_requested:
                self.worker.request_exit()
                self.db.close()
                sys.exit(0)
            # consume queues
            self.bin_queue_to_struct()
            if self.next_time >= datetime.now():
                self.next_time = datetime.now() + timedelta(hours=0, minutes=5)
                self.push_dicts_to_db()
                self.create_html()
            time.sleep(1.0)

    def bin_queue_to_struct(self):
        while self.bin_queue.qsize() > 0:
            bin_msg = self.bin_queue.get(block=True, timeout=None)
            if bin_msg.msgClass == 0:
                self.statistic_dic[bin_msg.stationId] = bin_msg

                # Todo we should send the relays as sensorData
                # relay_0 = self.decode_relay(0, bin_msg.relayStates)
                # relay_1 = self.decode_relay(1, bin_msg.relayStates)

            elif bin_msg.msgClass == 1:
                self.sensor_val_dic[bin_msg.stationId] = bin_msg
            self.bin_queue.task_done()

    def push_dicts_to_db(self):
        for msmnt in self.sensor_val_dic.values():
            self.db.update_sensordata(msmnt)
        for stats in self.statistic_dic.values():
            self.db.update_stationdata_tbl(stats)

    def create_html(self):
        ## ToDo hier weiter
        pass