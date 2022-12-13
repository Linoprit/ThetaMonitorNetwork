import logging
import wx


# ---------- set logging to wx.TextCtrl ----------
# https://stackoverflow.com/questions/2819791/how-can-i-redirect-the-logger-to-a-wxpython-textctrl-using-a-custom-logging-hand
def set_logger(txt_control: wx.TextCtrl):
    handler = WxTextCtrlHandler(txt_control)
    logging.getLogger().addHandler(handler)
    log_formatter = logging.Formatter(
        "%(asctime)s [%(levelname)-7.7s]  %(message)s", "%H:%M:%S")
    handler.setFormatter(log_formatter)


class WxTextCtrlHandler(logging.Handler):
    def __init__(self, ctrl):
        logging.Handler.__init__(self)
        self.ctrl = ctrl

    def emit(self, record):
        s = self.format(record) + '\n'
        wx.CallAfter(self.ctrl.WriteText, s)
