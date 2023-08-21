import logging
from datetime import datetime
import matplotlib as mpl
import matplotlib.pyplot as plt
from daemon.DataBaseConnector import DataBaseConnector as Dbcon
import framework.settings
from matplotlib.ticker import (MultipleLocator, AutoMinorLocator)
import matplotlib.ticker as ticker
# from matplotlib.ticker import (MultipleLocator, AutoMinorLocator, AutoLocator)


class PlotCreator:
    def __init__(self, settings_in: framework.settings.AppSettings, db_con: Dbcon):
        self.settings = settings_in
        self.db = db_con

    def plot_to_png(self, title, shortnames, from_str, till_str, filename):
        t_from = datetime.strptime(from_str, "%Y-%m-%d %H:%M:%S")
        t_till = datetime.strptime(till_str, "%Y-%m-%d %H:%M:%S")

        fig, ax = plt.subplots(figsize=(10, 6))
        plt.title(
            title, fontweight="bold", fontsize=20, loc='center', pad=50, color='#FF9900')
        date_form = mpl.dates.DateFormatter("%d.%m.%y %H:%M")
        ax.margins(0.01) # padding in all directions

        plt.xticks(rotation=90)
        plt.grid(which='major', color='grey',  linestyle=(1, (5, 5)), linewidth=0.5)
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

        ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.14),
          ncol=4, fancybox=True, shadow=True)
        plt.savefig(filename, bbox_inches='tight')
