import os
from datetime import date
from datetime import datetime
import signal
import sys
from queue import Queue
import serialIO.streamDecoder
import time
import logging
import framework.settings
from daemon.DataBaseConnector import DataBaseConnector as Dbcon
from daemon.PlotCreator import PlotCreator as PlotCreator
from serialIO.serialThread import Worker
from serialIO.streamDecoder import StreamDecoder as StreamDecoder
from daemon.HtmlCreator import HtmlCreator as Hc
from datetime import datetime, timedelta
import json
from pathlib import Path

#import daemon.HtmlCreator


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
    update_db_freq = 5 #  [min]
    # update_db_freq = 1 #  [min]

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
        self.plot_creator = PlotCreator(self.settings, self.db)
        self.next_time = \
            datetime.now() + timedelta(hours=0, minutes=self.update_db_freq)
        self.entry()

    def entry(self):
        device = self.settings.get("common", "serial_port")
        self.worker = Worker(self.serial_queue, device, 115200, 0.5)
        self.worker.start()
        self.db.connect()

        # TODO remove after devel
        self.create_html()

        # while 1:
        #     if self.exit_requested:
        #         self.worker.request_exit()
        #         self.db.close()
        #         sys.exit(0)
        #     # consume serial queues
        #     self.update_queues()
        #     self.bin_queue_to_struct()
        #     if datetime.now() >= self.next_time:
        #         self.next_time = \
        #             datetime.now() + timedelta(hours=0, minutes=self.update_db_freq)
        #         self.push_dicts_to_db()
        #         # self.create_html() TODO enable after devel
        #     time.sleep(1.0)  # we could sleep for update_db_freq

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
        self.create_plots()

        # html_creator = Hc(self.settings)
        # html_creator.create_testpage()

        # create plots
        # title = "Temperatur Werkstatt"
        # shortnames = ["WST_O", "WST_U"]
        # t_till = "2022-11-22 18:09:11"
        # t_from = "2022-11-21 18:04:11"
        # # shortnames = ["Test 003", "Test 008"]
        # # t_from = "2023-04-02 15:00:00"
        # # t_till = "2023-04-22 19:00:00"
        # filename = self.settings.add_temp_path(self.create_filename(title) + ".png")
        # self.plot_creator.plot_to_png(title, shortnames, t_from, t_till, filename)

    def create_plots(self):
        plot_conf_file = self.settings.expand(self.settings.get("daemon", "plot_config"))
        plot_dir = self.settings.expand(self.settings.get("daemon", "plot_dir"))

        # t_from_dt = datetime.today() - timedelta(days=1)
        # t_from = t_from_dt.strftime("%Y-%m-%d %H:%M:%S")
        # t_till = datetime.today().strftime("%Y-%m-%d %H:%M:%S")
        t_from = "2023-08-05 00:00:00"
        t_till = "2023-08-06 23:59:00"

        with open(plot_conf_file, 'r') as plt_config:
            configs = json.load(plt_config)
            for conf in configs:
                filename = self.create_filename(plot_dir, conf["title"])
                shortnames = conf["shortnames"]
                self.plot_creator.plot_to_png(
                    conf["title"], shortnames, t_from, t_till, filename)

    @staticmethod
    def create_filename(out_dir: str, plot_title: str):
        title_split = plot_title.split(' ')
        filename = str.join("_", title_split)
        return os.path.join(out_dir, plot_title)
