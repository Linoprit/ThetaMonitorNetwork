import logging
import binascii
import serial
from serial import SerialException


class SerialIO:
    def __init__(self, device, baudrate=115200, timeout=0.5):
        try:
            self.serial = serial.Serial(
                port=device, baudrate=baudrate, timeout=timeout)
            self._port_is_open = True
        except SerialException:
            logging.getLogger().error("Cannot open serial port " + device)
            self._port_is_open = False
        else:
            logging.getLogger().info("Opened port " + device)

    def close(self):
        self.serial.close()
        self._port_is_open = False
        logging.getLogger().info("Serial port closed.")

    def write_string(self, message: str):
        self.serial.write(message.encode())
        self.serial.flush()

    def read_string(self) -> str:
        data = self.serial.read(self.serial.inWaiting())
        return data.decode()

    def is_port_open(self):
        return self._port_is_open
