import unittest
from datetime import timedelta

from daemon.DataBaseConnector import DataBaseConnector as Dbcon
from daemon.HtmlConfigStructure import HtmlConfig
from daemon.HtmlCreator import HtmlCreator as HtmlCreator
from daemon.PlotCreator import PlotCreator as PlotCreator
from tests.unittest_setup import Unittest_setup


class TestCreatePlot(unittest.TestCase):
    setup = Unittest_setup()
    settings = setup.get_standard_settings()
    db = Dbcon(settings)
    db.connect()
    html_conf = HtmlConfig(settings)
    plot_creator = PlotCreator(settings, db, html_conf)
    html_creator = HtmlCreator(settings, db, html_conf)

    def test_create_sensor_plot_24h(self):
        title = "Temperatur Werkstatt"
        shortnames = [
            "WST_O   ",
            "WST_U   ",
            "GGE_U   ",
            "WST_O2  ",
            "WST_U2  ",
            "WrkstTmp",
            "LGRASN_U",
            "WrkstRel"
        ]
        t_till = Dbcon.str_to_datetime("2023-10-06 19:00:00")
        t_from = t_till - timedelta(days=1)
        filename = self.settings.add_temp_path("test_tempWst_24h")
        self.plot_creator.plot_to_png(title, shortnames, t_from, t_till, filename)

    def test_create_sensor_plot_48h(self):
        title = "Temperatur Werkstatt"
        shortnames = [
            "WST_O   ",
            "WST_U2  ",
            "WrkstTmp",
            "LGRASN_U",
            "WrkstRel"
        ]
        t_till = Dbcon.str_to_datetime("2023-10-06 19:00:00")
        t_from = t_till - timedelta(days=2)
        filename = self.settings.add_temp_path("test_tempWst_48h")
        self.plot_creator.plot_to_png(title, shortnames, t_from, t_till, filename)

    def test_create_sensor_plot_1_week(self):
        title = "Temperatur Werkstatt"
        shortnames = [
            "WST_O   ",
            "WST_U2  ",
            "WrkstTmp",
            "LGRASN_U",
            "WrkstRel"
        ]
        t_till = Dbcon.str_to_datetime("2023-10-06 19:00:00")
        t_from = t_till - timedelta(days=7)
        filename = self.settings.add_temp_path("test_tempWst_1_week")
        self.plot_creator.plot_to_png(title, shortnames, t_from, t_till, filename)
