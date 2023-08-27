import logging
import os
from datetime import datetime
import matplotlib as mpl
import matplotlib.pyplot as plt
from daemon.DataBaseConnector import DataBaseConnector as Dbcon
import framework.settings
from matplotlib.ticker import (MultipleLocator, AutoMinorLocator)
from pathlib import Path
from daemon.HtmlConfigStructure import HtmlConfig
from datetime import datetime, timedelta


class PlotCreator:
    def __init__(self,
                 settings_in: framework.settings.AppSettings,
                 db_con: Dbcon,
                 html_conf: HtmlConfig):
        self.settings = settings_in
        self.db = db_con
        self.plot_config = html_conf.get_plot_config()
        self.content_config = html_conf.get_content_config()
        self.plot_dir = self.check_or_create_plot_dir()

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
            delta = conf["time_back"].split("=")[1]
            t_from_dt = datetime.today() - timedelta(days=int(delta))
            t_till_dt = datetime.today()
            # t_from = t_from_dt.strftime("%Y-%m-%d %H:%M:%S")
            # t_till = datetime.today().strftime("%Y-%m-%d %H:%M:%S")
            content_config_name = conf["name"]
            self.do_sensor_plots(content_config_name, t_from_dt, t_till_dt)
            self.do_statistik_plot(t_from_dt, t_till_dt)

    def do_statistik_plot(self, t_from, t_till):
        db_result = self.db.get_statistikdata(t_from, t_till)
        if db_result is None:
            logging.getLogger().info(
                "No stat-data found from {} to {}"
                .format(t_from.strftime("%Y-%m-%d %H:%M:%S"),
                        t_till.strftime("%Y-%m-%d %H:%M:%S")))
            return
        # else...

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
        # t_from = datetime.strptime(from_str, "%Y-%m-%d %H:%M:%S")
        # t_till = datetime.strptime(till_str, "%Y-%m-%d %H:%M:%S")
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

        secax = ax.secondary_yaxis('right')
        secax.yaxis.set_major_formatter('{x:1.1f}')
        secax.yaxis.set_minor_locator(AutoMinorLocator())

        for shortname in shortnames:
            db_result = self.db.get_single_sensordata(shortname, t_from, t_till)
            if db_result is None:
                logging.getLogger().info("No data found for " + shortname)
                return
            if "Rel" in shortname:
                x2 = []
                y2 = []
                for item in db_result:
                    x2.append(item[0])
                    y2.append(item[1])
                ax2 = ax.twinx()
                ax2.axis('off')
                ax2.plot(x2, y2, label=shortname, linewidth=0.5)
                x2.clear()
                y2.clear()
                continue

            x1 = []
            y1 = []
            for item in db_result:
                x1.append(item[0])
                y1.append(item[1])
            ax.plot(x1, y1, label=shortname, linewidth=1.0)
            x1.clear()
            y1.clear()

        ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.14), ncol=4, fancybox=True, shadow=True)
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
