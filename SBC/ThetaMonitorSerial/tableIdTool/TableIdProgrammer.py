import logging

import gui.Subparser
import tableIdTool.Subparser
from framework.filehelper import Filehelper as Fh
from framework.settings import AppSettings as AppSettings
from serialIO import SerialIO as Sio
from tableIdTool.CommandMap import CommandMap
from tableIdTool.CommandTransmitter import CommandTransmitter
from tableIdTool.IdTableParser import IdTableParser as IdTableParser


class TableIdProgrammer:
    def __init__(self, settings_in: AppSettings):
        self.settings = settings_in
        self.args = self.settings.args
        tableIdTool.Subparser.overlay_settings(self.settings)
        gui.Subparser.overlay_settings(self.settings)
        serial_io = Sio.SerialIO(self.settings.get('common', 'serial_port'))
        self.cmd_trnsmtr = CommandTransmitter(self.settings, serial_io)
        cmd_map = CommandMap()
        if self.args.tableIdFile is not None:
            if self.args.tableIdFile == 'default':
                id_file = Fh.read_file_to_list(
                    self.settings.get_as_path("table_id_tool", "table_id_file"))
            else:
                id_file = Fh.read_file_to_list(self.args.tableIdFile)
            id_parser = IdTableParser()
            id_parser.parse(id_file)
            id_table = id_parser.get_with_command(cmd_map.getCmdString('setSensIdCmd'))
            self.transmit([cmd_map.getCmdString('shutupCmd') + chr(13)])
            self.transmit(id_table)
            self.transmit_no_ack([cmd_map.getCmdString('rebootCmd') + chr(13)])
            # [print(x) for x in id_table]
        elif self.args.stationId is not None:
            cmd = [cmd_map.getCmdString('setStationIdCmd') +
                   " " + str(self.args.stationId) + chr(13)]
            self.transmit(cmd)
        elif self.args.readIdTable:
            cmd = [cmd_map.getCmdString('getSensIdTableCmd') + chr(13)]
            self.transmit(cmd)
        elif self.args.getstationid is not None:
            cmd = [cmd_map.getCmdString('getStationIdCmd') + chr(13)]
            self.transmit(cmd)
        else:
            print("You must specify an action to be done. For help"
                  " type '" + self.settings.get('common', 'app_name') +
                  " prog -h'.")

    def transmit_no_ack(self, cmd_list: list):
        self.cmd_trnsmtr.transmit_no_ack(cmd_list)

    def transmit(self, cmd_list: list):
        result = self.cmd_trnsmtr.transmit_commands(cmd_list)
        if not result:
            logging.getLogger().info("Transmitting commands unsuccessful.")
