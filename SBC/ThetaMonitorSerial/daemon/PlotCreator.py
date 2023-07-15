import logging
from datetime import datetime
import matplotlib as mpl
import matplotlib.pyplot as plt
from daemon.DataBaseConnector import DataBaseConnector as Dbcon
import framework.settings
from matplotlib.ticker import (MultipleLocator)
# from matplotlib.ticker import (MultipleLocator, AutoMinorLocator, AutoLocator)


class PlotCreator:
    def __init__(self, settings_in: framework.settings.AppSettings, db_con: Dbcon):
        self.settings = settings_in
        self.db = db_con

    def plot_to_png(self, title, shortnames, from_str, till_str, filename):
        t_from = datetime.strptime(from_str, "%Y-%m-%d %H:%M:%S")
        t_till = datetime.strptime(till_str, "%Y-%m-%d %H:%M:%S")

        fig, ax = plt.subplots(figsize=(10, 6))
        for shortname in shortnames:
            db_result = self.db.get_single_sensordata(shortname, t_from, t_till)
            x1 = []
            y1 = []
            for item in db_result:
                x1.append(item[0])
                y1.append(item[1])
            ax.plot(x1, y1, label=shortname, linewidth=1.0)
            x1.clear()
            y1.clear()

        plt.title(
            title, fontweight="bold", fontsize=20, loc='center', pad=10, color='#FF9900')
        # plt.ylabel("Temp [°C]", fontdict=None, labelpad=None, loc=None)
        date_form = mpl.dates.DateFormatter("%d.%m.%y %H:%M")
        ax.xaxis.set_major_formatter(date_form)
        ax.xaxis.set_major_locator(MultipleLocator(0.02))
        # ax.xaxis.set_minor_locator(AutoMinorLocator())
        ax.secondary_yaxis('right')
        plt.xticks(rotation=90)
        plt.grid(color='grey', linestyle=(1, (5, 5)), linewidth=0.5)
        ax.legend(loc='upper left')
        plt.savefig(filename, bbox_inches='tight')

        logging.info("here")