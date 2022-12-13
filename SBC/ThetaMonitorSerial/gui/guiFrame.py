import wx
from serial.tools import list_ports

import gui.ThetaMonitorGui as Tmg
import framework.guiHelpers as guiHelpers
import framework.settings
import logging

import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).parent))


# Implementing MainFrame
class MainFrame(Tmg.ThetaMonitorFrame):
    def __init__(self, parent, settings_in: framework.settings.AppSettings):
        self.settings = settings_in
        Tmg.ThetaMonitorFrame.__init__(self, parent)
        guiHelpers.set_logger(self.m_textCtrlStatus)

        self._init_serial_combo()
        # self.timer = wx.Timer(self)
        # self.Bind(wx.EVT_TIMER, self.update, self.timer)

    # ---------- Helpers ----------
    def _init_serial_combo(self):
        portlist = list_ports.comports()
        self.m_choiceSerialPort.Clear()
        [self.m_choiceSerialPort.Append(x.device) for x in portlist]
        self.m_choiceSerialPort.SetSelection(0)
        index = self.m_choiceSerialPort.FindString(
            self.settings.get('common', 'serial_port'))
        if index != wx.NOT_FOUND:
            self.m_choiceSerialPort.SetSelection(index)

    # ---------- Callbacks ----------
    def on_quit_button_click(self, event):
        event.Skip()
        self.Close()
