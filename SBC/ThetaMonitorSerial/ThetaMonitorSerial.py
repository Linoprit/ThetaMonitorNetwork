import framework.settings
from serialIO import serialThread as Thr
import wx
import gui.classThetaMonGui as Mf

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
        frame = Mf.ThetaMonGui(None, settings)
        frame.Show(True)
        app.MainLoop()
    # elif what_to_run == 'SERIALIO':
    #    Sio.entry()  # testcode
    elif what_to_run == 'THREAD':
        Thr.main()
        # Seh.main()

    else:
        print("No serialIO selected in settings.ini.")

# Desiderata:
# Log messages from monitor, Button on off
# Monitor filter Buttons txt/bin
# Console with history and help
# SensorTable Editor with load and save Table-File
# SensorTable Write to E2 Button in Console-Win
# SensorTable Write/Read to/from DB
# Writing received data to Database
# Get Data from DB and build graphs
# somehow track sensors and show timeout
# Headless mode

