import pathlib
import sys
from queue import Queue
import numpy as np

import wx
import wx.html
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

        self.Bind(
            event=wx.EVT_TEXT_ENTER, handler=self.on_textCtrl_com_line_enter)
        self.Bind(wx.EVT_CHAR_HOOK, self.onKey)

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
        configured_port = self.settings.get('common', 'serial_port')
        self.m_choiceSerialPort.Clear()
        [self.m_choiceSerialPort.Append(x.device) for x in portlist]
        self.m_choiceSerialPort.SetSelection(0)
        index = self.m_choiceSerialPort.FindString(configured_port)
        if index == wx.NOT_FOUND:
            self.m_choiceSerialPort.Append(configured_port)
            self.m_choiceSerialPort.SetSelection(0)
        else:
            self.m_choiceSerialPort.SetSelection(index)

    def txt_queue_to_console(self):
        text = ""
        while self.txt_queue.qsize() > 0:
            text += StreamDecoder.get_txt_queue_as_string(self.txt_queue)
        if text and self.m_toggleBtnFilterTxt.GetValue():
            text = self.align_to_html(text)
            self.shorten_html_content(self.m_htmlWin_console, len(text))
            self.m_htmlWin_console.AppendToPage(text)
            self.scroll_htmlMonitor_down()

    def bin_queue_to_console(self):
        text = "<TT>"
        while self.bin_queue.qsize() > 0:
            bin_msg = self.bin_queue.get(block=True, timeout=None)
            if bin_msg.msgClass == 0:
                text += " class: " + str(bin_msg.msgClass)
                text += " station: " + str(bin_msg.stationId)
                text += " tick: " + str(bin_msg.lastUpdateTick)
                text += " SnsIdHash: " + str(bin_msg.sensorIdHash)
                text += " value: " + str(np.around(bin_msg.value, decimals=2))
                text += " chksum: " + str(bin_msg.checksum)
                text += "<br>"
            elif bin_msg.msgClass == 1:
                text += " class: " + str(bin_msg.msgClass)
                text += " station: " + str(bin_msg.stationId)
                text += " relays: " + str(bin_msg.relayStates)
                text += " lostPkgs: " + str(bin_msg.lostPkgs)
                text += " validSensors: " + str(bin_msg.validSensors)
                text += " rxBufferOverflows: " + str(bin_msg.rxBufferOverflows)
                text += " lastUpdateTick: " + str(bin_msg.lastUpdateTick)
                text += " chksum: " + str(bin_msg.checksum)
                text += "<br>"
            self.bin_queue.task_done()
        if self.m_toggleBtnFilterBin.GetValue():
            text += "<br></TT>"
            self.shorten_html_content(self.m_htmlWin_console, len(text))
            self.m_htmlWin_console.AppendToPage(text)
            self.scroll_htmlMonitor_down()

    def align_to_html(self, text) -> str:
        text = text.replace('\n', "<br>")
        text = text.replace('\t', "&nbsp;&nbsp;&nbsp;&nbsp;")
        text = text.replace(' ', "&nbsp;")
        text = "<TT>" + text + "</TT>"
        return text

    def shorten_html_content(self, html_win: wx.html.HtmlWindow, textlen):
        text = self.m_htmlWin_console.ToText()
        html_len = len(text)
        if html_len < 10000:
            return
        new_text = text[textlen: -1:]
        line_break_pos = new_text.find("\n")
        new_text = new_text[line_break_pos: -1:]
        new_text = self.align_to_html(new_text) + "<br>"
        html_win.SetPage(new_text)

    def scroll_htmlMonitor_down(self):
        # if self.m_bmToggleBtnAutoScroll.GetValue():
        maxrange = self.m_htmlWin_console.GetScrollRange(wx.VERTICAL)
        pagesize = self.m_htmlWin_console.GetScrollPageSize(wx.VERTICAL)
        self.m_htmlWin_console.Scroll(-1, maxrange - pagesize)

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

    def on_button_clr_pushed(self, event):
        self.m_htmlWin_console.SetPage("")
        event.Skip()

    def on_button_filter_bin_toggled(self, event):
        event.Skip()

    def on_button_filter_txt_toggled(self, event):
        event.Skip()

    def on_textCtrl_com_line_enter(self, event):
        event.Skip()
        print("text enter")

    def onKey(self, evt):
        if evt.GetKeyCode() == wx.WXK_DOWN:
            print("Down key pressed")
        elif evt.GetKeyCode() == wx.WXK_UP:
            print("Up key pressed")
        else:
            evt.Skip()
