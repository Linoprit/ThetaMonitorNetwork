import pathlib
import sys
from _queue import Empty
from queue import Queue
import numpy as np
import serial

import wx
from serial.tools import list_ports

import framework.guiHelpers as guiHelpers
import framework.settings
import gui.ThetaMonitorGui as Tmg
from serialIO.serialThread import Worker
from serialIO.streamDecoder import StreamDecoder as StreamDecoder

sys.path.insert(0, str(pathlib.Path(__file__).parent))


# Implementing MainFrame
class ThetaMonGui(Tmg.ThetaMonitorFrame):
    def __init__(self, parent, settings_in: framework.settings.AppSettings):
        self.settings = settings_in
        Tmg.ThetaMonitorFrame.__init__(self, parent)
        guiHelpers.set_logger(self.m_textCtrlStatus)

        self._init_serial_combo()
        self.serial_queue = Queue()
        self.txt_queue = Queue()
        self.bin_queue = Queue()
        self.stream_decoder = StreamDecoder(self.txt_queue, self.bin_queue)
        self.worker = None
        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.update_gui, self.timer)
        self.timer.Start(milliseconds=1000, oneShot=wx.TIMER_CONTINUOUS)

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

    def txt_queue_to_console(self):
        text = ""
        while self.txt_queue.qsize() > 0:
            text += StreamDecoder.get_txt_queue_as_string(self.txt_queue)
        if text:
            text = text.replace('\n', "<br>")
            text = text.replace('\t', "     ")
            self.m_htmlWinMonitor.AppendToPage(text)
            self.scroll_htmlMonitor_down()

    def bin_queue_to_console(self):
        text = ""
        while self.bin_queue.qsize() > 0:
            bin_msg = self.bin_queue.get(block=True, timeout=None)
            text += " class: " + str(bin_msg.msgClass)
            text += " station: " + str(bin_msg.stationId)
            text += " tick: " + str(bin_msg.lastUpdateTick)
            text += " SnsIdHash: " + str(bin_msg.sensorIdHash)
            text += " value: " + str(np.around(bin_msg.value, decimals=2))
            text += " chksum: " + str(bin_msg.checksum)
            text += "<br>"
            self.bin_queue.task_done()
        text += "<br>"
        self.m_htmlWinMonitor.AppendToPage(text)
        self.scroll_htmlMonitor_down()

    def scroll_htmlMonitor_down(self):
        if self.m_bmToggleBtnAutoScroll.GetValue():
            maxrange = self.m_htmlWinMonitor.GetScrollRange(wx.VERTICAL)
            pagesize = self.m_htmlWinMonitor.GetScrollPageSize(wx.VERTICAL)
            self.m_htmlWinMonitor.Scroll(-1, maxrange - pagesize)

    # ---------- Callbacks ----------
    def update_gui(self, event):
        event.Skip()
        data = None
        for j in range(self.serial_queue.qsize() - 1):
            data = self.serial_queue.get(block=True, timeout=0.2)
        if data is None:
            return
        self.stream_decoder.proc_serial_stream(data)
        self.txt_queue_to_console()
        self.bin_queue_to_console()

    def on_quit_button_click(self, event):
        if self.worker is not None:
            self.worker.request_exit()
            self.worker.join()
        event.Skip()
        self.Close()

    def on_button_connect_toggled(self, event):
        if self.m_bmToggleBtnConnect.GetValue():
            device = self.m_choiceSerialPort.GetStringSelection()
            self.worker = Worker(self.serial_queue, device, 115200, 0.5)
            self.worker.start()
        else:
            if self.worker is not None:
                self.worker.request_exit()

    def on_choice_serial_port(self, event):
        self.m_bmToggleBtnConnect.SetValue(False)
        self.on_button_connect_toggled(event)

    def on_RefreshCombo(self, event):
        self.m_bmToggleBtnConnect.SetValue(False)
        self.on_button_connect_toggled(event)
        self._init_serial_combo()
