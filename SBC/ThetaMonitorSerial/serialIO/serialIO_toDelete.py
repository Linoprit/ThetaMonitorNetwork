import framework.settings
import asyncio
import serial_asyncio
import time
from queue import Queue

queue = Queue()


# https://pyserial-asyncio.readthedocs.io/en/latest/shortintro.html#protocol-example
def entry():
    loop = asyncio.get_event_loop()
    loop.run_until_complete(reader(loop))
    loop.close()


async def reader(loop_: asyncio.events):
    transport, protocol = \
        await serial_asyncio.create_serial_connection(
            loop_, SerialIO, '/dev/ttyUSB0', baudrate=115200)

    while True:
        await asyncio.sleep(0.3)

        # time.sleep(0.3)
        if not queue.empty():
            data = queue.get_nowait()
            print()
            #print('data received', repr(data))

        protocol.resume_reading()


class SerialIO(asyncio.Protocol):
    # def __init__(self, app_settings: framework.settings.AppSettings):
    def __init__(self):
        # self.app_settings = app_settings
        self.transport = None

    def connection_made(self, transport):
        self.transport = transport

    def data_received(self, data):
        # stop callbacks again immediately
        self.pause_reading()
        try:
            queue.put_nowait(data)
        except queue.Full:
            pass
        [print(str(x) + ", ", end='') for x in data]
        # print('data received', repr(data))

    def pause_reading(self):
        # This will stop the callbacks to data_received
        self.transport.pause_reading()

    def resume_reading(self):
        # This will start the callbacks to data_received again
        # with all data that has been received in the meantime.
        self.transport.resume_reading()
