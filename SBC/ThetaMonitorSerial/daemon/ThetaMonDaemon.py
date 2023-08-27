import logging
import signal
import sys
import time
from datetime import datetime, timedelta
from queue import Queue

import framework.settings
import serialIO.streamDecoder
from daemon.DataBaseConnector import DataBaseConnector as Dbcon
from daemon.HtmlConfigStructure import HtmlConfig
from daemon.HtmlCreator import HtmlCreator as HtmlCreator
from daemon.PlotCreator import PlotCreator as PlotCreator
from serialIO.serialThread import Worker
from serialIO.streamDecoder import StreamDecoder as StreamDecoder


# Desiderata:
# Ungültige Sensordaten kennzeichnen und nicht mehr in die db schieben
#   (ticks checken und wenn zu alt aus liste löschen)


# ctrl+c signal handler
def signal_handler(sig, frame):
    print('Exit requested, waiting for all threads to exit.')
    ThetaMonDaemon.exit_requested = True


class ThetaMonDaemon:
    exit_requested = False
    update_db_freq = 5  # [min]

    # update_db_freq = 1 #  [min]

    def __init__(self, settings_in: framework.settings.AppSettings):
        self.settings = settings_in
        self.serial_queue = Queue()
        self.txt_queue = Queue()
        self.bin_queue = Queue()
        self.stream_decoder = StreamDecoder(self.txt_queue, self.bin_queue)
        self.sensor_val_dic = {}  # { sensorId, MeasurementType }
        self.statistic_dic = {}  # { sensorId, RadioStatisticsType }
        self.html_conf = HtmlConfig(self.settings)
        self.worker = None
        signal.signal(signal.SIGINT, signal_handler)
        self.db = Dbcon(self.settings)
        self.next_time = \
            datetime.now() + timedelta(hours=0, minutes=self.update_db_freq)
        self.plot_creator = PlotCreator(self.settings, self.db, self.html_conf)
        self.html_creator = HtmlCreator(self.settings, self.db, self.html_conf)
        self.entry()

    def entry(self):
        device = self.settings.get("common", "serial_port")
        self.worker = Worker(self.serial_queue, device, 115200, 0.5)
        self.worker.start()
        self.db.connect()
        self.create_html()  # initial creation

        while 1:
            if self.exit_requested:
                self.worker.request_exit()
                self.db.close()
                sys.exit(0)
            # consume serial queues
            self.update_queues()
            self.bin_queue_to_struct()
            if datetime.now() >= self.next_time:
                self.next_time = \
                    datetime.now() + timedelta(hours=0, minutes=self.update_db_freq)
                self.push_dicts_to_db()
                self.create_html()
            time.sleep(1.0)  # we could sleep for update_db_freq

    def update_queues(self):
        data = None
        for j in range(self.serial_queue.qsize() - 1):
            data = self.serial_queue.get(block=True, timeout=0.2)
        if data is None:
            return
        self.stream_decoder.proc_serial_stream(data)

    def bin_queue_to_struct(self):
        while self.bin_queue.qsize() > 0:
            bin_msg = self.bin_queue.get(block=True, timeout=None)
            if bin_msg.msgClass == serialIO.streamDecoder.MEASUREMENT_ENUM:
                self.sensor_val_dic[bin_msg.sensorIdHash] = bin_msg
            elif bin_msg.msgClass == serialIO.streamDecoder.STATISTICS_ENUM:
                self.statistic_dic[bin_msg.stationId] = bin_msg
            self.bin_queue.task_done()

    def push_dicts_to_db(self):
        sens_dat_count = 0
        stat_dat_count = 0
        for msmnt in self.sensor_val_dic.values():
            self.db.update_sensordata(msmnt)
            sens_dat_count += 1
        for stats in self.statistic_dic.values():
            self.db.update_stationdata_tbl(stats)
            stat_dat_count += 1
        self.statistic_dic.clear()
        self.sensor_val_dic.clear()
        if sens_dat_count > 0:
            logging.getLogger().info(
                "Updated sensordata with {} items".format(sens_dat_count))
        if stat_dat_count > 0:
            logging.getLogger().info(
                "Updated stationdata with {} items.".format(stat_dat_count))

    def create_html(self):
        start = time.time()
        self.plot_creator.create_plots()
        end = time.time()
        logging.getLogger().info("Time needed to create plots: {}"
                                 .format(end - start))
        self.html_creator.create_html_pages()

