from queue import Queue
from threading import Thread
import time
import serial
import logging

from serial import SerialException


class Worker(Thread):
    def __init__(self, serial_queue: Queue, device, baudrate, timeout):
        super(Worker, self).__init__()
        try:
            self.serial = serial.Serial(
                port=device, baudrate=baudrate, timeout=timeout)
        except SerialException:
            logging.getLogger().error("Cannot open serial port " + device)
            self._exit_is_requested = True
        logging.getLogger().info("Opened port " + device)
        self._exit_is_requested = False
        self._serial_queue = serial_queue

    def run(self):
        logging.getLogger().info("Starting serial thread.")
        self.loop()

    def request_exit(self):
        self._exit_is_requested = True

    def loop(self):
        while not self._exit_is_requested:
            try:
                data = self.serial.read(self.serial.inWaiting())
                if data:
                    self._serial_queue.put(data, block=True, timeout=0.5)
            except:
                logging.getLogger().info("Connection lost. Exiting serial thread.")
                return
            time.sleep(1.0)
        # some exit code
        logging.getLogger().info("Exiting serial thread.")
        self.serial.close()
