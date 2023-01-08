import framework.settings
import tableIdTool.TableIdProgrammer as Tip
import wx
import gui.classThetaMonGui as Mf

if __name__ == '__main__':
    settings = framework.settings.AppSettings()
    settings.set_name_and_version('ThetaMonitor', 'v000.001.001')
    settings.print_status()

    if settings.args.tool_name == 'gui':
        app = wx.App(False)
        frame = Mf.ThetaMonGui(None, settings)
        frame.Show(True)
        app.MainLoop()
    elif settings.args.tool_name == 'prog':
        tip = Tip.TableIdProgrammer(settings)
    else:
        # Todo run the deamon
        print("Deamon not implemented")


# Desiderata:
# Log messages from monitor, Button on off
# * Monitor filter Buttons txt/bin
# Console with history and help
# SensorTable Editor with load and save Table-File
# SensorTable Write to E2 Button in Console-Win
# SensorTable Write/Read to/from DB
# Writing received data to Database
# Get Data from DB and build graphs
# somehow track sensors and show timeout
# Headless mode
