import logging
import os
from datetime import timedelta, datetime
from itertools import cycle
from pathlib import Path

import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.ticker import (MultipleLocator, AutoMinorLocator)

import framework.settings
from daemon.DataBaseConnector import DataBaseConnector as Dbcon
from daemon.HtmlConfigStructure import HtmlConfig

mpl.use('Agg')


class PlotCreator:
    def __init__(self,
                 settings_in: framework.settings.AppSettings,
                 db_con: Dbcon,
                 html_conf: HtmlConfig):
        self.settings = settings_in
        self.db = db_con
        self.statistic_config = html_conf.get_statistic_config()
        self.plot_config = html_conf.get_plot_config()
        self.content_config = html_conf.get_content_config()
        self.plot_dir = self.check_or_create_plot_dir()
        self.this_midnight = self.get_midnight() - timedelta(days=1)  # first run

    def check_or_create_plot_dir(self):
        html_outdir = self.settings.expand(
            self.settings.get("daemon", "html_outdir"))
        plot_dir = os.path.join(html_outdir, "plots")
        if not Path(plot_dir).is_dir():
            os.makedirs(plot_dir)
            logging.getLogger().info("Created plot-path: " + plot_dir)
        return plot_dir

    def create_plots(self):
        for conf in self.content_config:
            content_config_name = conf["name"]
            elapsed = datetime.now() - self.this_midnight
            if content_config_name == "1_Week" or content_config_name == "48h":
                if elapsed.days < 1:
                    continue
            delta = conf["time_back"].split("=")[1]
            # t_till_dt = Dbcon.str_to_datetime("2023-08-06 19:30:09")
            t_till_dt = datetime.today()
            t_from_dt = t_till_dt - timedelta(days=int(delta))
            self.do_sensor_plots(content_config_name, t_from_dt, t_till_dt)
            self.do_statistik_plot(content_config_name, t_from_dt, t_till_dt)
        self.this_midnight = self.get_midnight()

    @staticmethod
    def get_midnight():
        now = datetime.now()
        return datetime(now.year, now.month, now.day, 0, 0, 0)

    def do_statistik_plot(self, content_config_name, t_from, t_till):
        if self.statistic_config is None:
            return
        data_names = self.statistic_config["data_names"]
        filename = (self.create_fullpath(
                self.plot_dir, self.statistic_config["name"], content_config_name))
        station_ids = self.db.get_station_ids_from_statistik(t_from, t_till)
        ax, ax2, colors = self.prepare_plot("Station Statistics", sec_axis_visible=True)
        lns = []
        x1 = []
        y1 = []
        y2 = []
        for stat_id in station_ids:
            db_result = self.db.get_station_stats(stat_id, data_names, t_from, t_till)
            for station_data in db_result:
                x1.append(station_data[0])
                y1.append(station_data[1])
                y2.append(station_data[2])
            label_ax = "({}) {}".format(str(stat_id), data_names[0])
            label_ax2 = "({}) {}".format(str(stat_id), data_names[1])
            lns += ax.plot(x1, y1, color=next(colors), label=label_ax, linewidth=1.0)
            lns += ax2.plot(x1, y2, color=next(colors), label=label_ax2, linewidth=1.0)
            x1.clear()
            y1.clear()
            y2.clear()
        self.make_legend(lns, ax)
        self.save_plot(filename, ax)

    def do_sensor_plots(self, content_config_name, t_from, t_till):
        if self.plot_config is None:
            return
        for conf in self.plot_config:
            filename = (self.create_fullpath(
                self.plot_dir, conf["name"], content_config_name))
            shortnames = conf["shortnames"]
            self.plot_to_png(
                conf["title"], shortnames, t_from, t_till, filename)

    def plot_to_png(self, title, shortnames, t_from, t_till, filename):
        ax, ax2, colors = self.prepare_plot(title)
        lns = []
        for shortname in shortnames:
            db_result = self.db.get_single_sensordata(shortname, t_from, t_till)
            if (db_result is None) or (len(db_result) == 0):
                logging.getLogger().info("No data found for " + shortname)
                continue
            if "Rel" in shortname:
                x2 = []
                y2 = []
                for item in db_result:
                    x2.append(item[0])
                    y2.append(item[1])
                lns += ax2.plot(x2, y2, color=next(colors), label=shortname, linewidth=0.5)
                x2.clear()
                y2.clear()
                continue

            x1 = []
            y1 = []
            for item in db_result:
                x1.append(item[0])
                y1.append(item[1])
            lns += ax.plot(x1, y1, color=next(colors), label=shortname, linewidth=1.0)
            x1.clear()
            y1.clear()
        self.make_legend(lns, ax)
        self.save_plot(filename, ax)

    @staticmethod
    def prepare_plot(title, sec_axis_visible=False):
        fig, ax = plt.subplots(figsize=(10, 6))
        plt.title(
            title, fontweight="bold", fontsize=20, loc='center', pad=50, color='#FF9900')
        date_form = mpl.dates.DateFormatter("%d.%m.%y %H:%M")
        ax.margins(0.01)  # padding in all directions

        plt.xticks(rotation=90)
        plt.grid(which='major', color='grey', linestyle=(1, (5, 5)), linewidth=0.5)
        plt.grid(which='minor', color='lightgrey', linestyle='--', linewidth=0.5)

        ax.xaxis.set_major_formatter(date_form)
        ax.xaxis.set_major_locator(MultipleLocator(0.03))
        ax.xaxis.set_minor_locator(MultipleLocator(0.015))

        ax.yaxis.set_major_formatter('{x:1.1f}')
        ax.yaxis.set_minor_locator(AutoMinorLocator())

        ax2 = ax.twinx()
        if sec_axis_visible:
            ax2.axis('on')
        else:
            secax = ax.secondary_yaxis('right')
            secax.yaxis.set_major_formatter('{x:1.1f}')
            secax.yaxis.set_minor_locator(AutoMinorLocator())
            ax2.axis('off')

        prop_cycle = plt.rcParams['axes.prop_cycle']
        colors = cycle(prop_cycle.by_key()['color'])
        return ax, ax2, colors

    @staticmethod
    def make_legend(lines, ax):
        labs = [line.get_label() for line in lines]
        ax.legend(lines, labs, loc='upper center', bbox_to_anchor=(0.5, 1.14), ncol=4, fancybox=True, shadow=True)

    @staticmethod
    def save_plot(filename, ax):
        plt.savefig(filename, bbox_inches='tight')
        plt.close('all')

    @staticmethod
    def create_fullpath(out_dir: str, plot_name: str, content_config_name: str):
        filename = PlotCreator.create_filename(plot_name, content_config_name)
        return os.path.join(out_dir, filename)

    @staticmethod
    def create_filename(plot_name: str, content_config_name: str):
        name = str.join("_", [plot_name, content_config_name])
        return name + ".png"
