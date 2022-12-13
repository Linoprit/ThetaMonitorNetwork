import framework.settings
from serialIO import serialIO as Sio
from serialIO import Thread as Thr
from serialIO import serialHelpers as Seh
import wx
import gui.guiFrame as Mf


if __name__ == '__main__':
    settings = framework.settings.AppSettings()
    settings.set_name_and_version('ThetaMonitor', 'v000.001.001')
    settings.print_status()

    # later, we have the SBC configuration
    # headless = settings.get('options', 'headless', uppercase=True)

    what_to_run = settings.get('devel', 'start', uppercase=True)
    if what_to_run == 'SBC_TARGET':
        print("to be implemented")
    elif what_to_run == 'GUI_TARGET':
        app = wx.App(False)
        frame = Mf.MainFrame(None, settings)
        frame.Show(True)
        app.MainLoop()
    elif what_to_run == 'SERIALIO':
        Sio.entry()  # testcode
    elif what_to_run == 'THREAD':
        Thr.main()
        # Seh.main()

    else:
        print("No serialIO selected in settings.ini.")
