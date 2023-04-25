import logging

import framework.settings
import tableIdTool.TableIdProgrammer as Tip
import wx
import gui.classThetaMonGui as Mf
import daemon.ThetaMonDaemon as Dm

if __name__ == '__main__':
    settings = framework.settings.AppSettings()
    settings.set_name_and_version('ThetaMonitor', 'v000.001.001')
    settings.print_status()

    what_to_run = settings.get('common', 'tool_name')
    if what_to_run == 'gui':
        app = wx.App(False)
        frame = Mf.ThetaMonGui(None, settings)
        frame.Show(True)
        app.MainLoop()
    elif what_to_run == 'prog':
        tip = Tip.TableIdProgrammer(settings)
    elif what_to_run == 'daemon':
        daemon = Dm.ThetaMonDaemon(settings)
        daemon.entry()
    elif what_to_run == 'devel':
        pass
    else:
        logging.error("No tool_name specified.")


# Desiderata:
# Console with history and help
# SensorTable Editor with load and save Table-File and export to .txt
# SensorTable Write to E2 Button in Console-Win
# SensorTable Write/Read to/from DB
# Get Data from DB and build graphs
# somehow track sensors and show timeout
# Headless / Deamon mode

