from serialIO import SerialIO as SerialIO
import logging
from framework.settings import AppSettings as AppSettings
import time


class CommandTransmitter:
    def __init__(self,  settings_in: AppSettings, serial_io: SerialIO):
        self.settings = settings_in
        self.sio = serial_io

    def transmit_commands(self, commands: list) -> bool:  # list[str]
        if not self.sio.is_port_open():
            logging.getLogger().error("Serial port is not open!")
            return False
        count = 0
        ack_ok = False
        waitstates = self.settings.get("table_id_tool", "waitstates")
        answer = ""
        to_find = self.settings.get("table_id_tool", "acknowledge")
        for item in commands:
            print("SENDING: " + item)
            self.sio.write_string(item)

            while (count < waitstates) or ack_ok :
                answer = self.sio.read_string()
                if to_find in answer:
                    ack_ok = True
                    print(" OK")
                    break
                time.sleep(0.5)
                count += 1
                print(".", end='')

            if not ack_ok:
                return False
            print(answer)
        return True
