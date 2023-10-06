import logging

import framework.settings
import wx
import daemon.ThetaMonDaemon as Dm

if __name__ == '__main__':
    settings = framework.settings.AppSettings()
    settings.set_name_and_version('ThetaMonitor', 'v000.001.001')
    settings.print_status()

    daemon = Dm.ThetaMonDaemon(settings)
    daemon.entry()
