import signal
import sys

from queue import Queue
import framework.settings
from serialIO.serialThread import Worker
from serialIO.streamDecoder import StreamDecoder as StreamDecoder

# ctrl+c signal handler
def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    sys.exit(0)

class ThetaMonDeamon:
    def __init__(self, settings_in: framework.settings.AppSettings):
        self.settings = settings_in
        self.serial_queue = Queue()
        self.txt_queue = Queue()
        self.bin_queue = Queue()
        self.stream_decoder = StreamDecoder(self.txt_queue, self.bin_queue)
        self.worker = None
        signal.signal(signal.SIGINT, signal_handler)
        self.entry()

    def entry(self):
        device = self.settings.get("common", "serial_port")
        self.worker = Worker(self.serial_queue, device, 115200, 0.5)
        self.worker.start()
        while 1:
            pass





# signal.signal(signal.SIGINT, signal_handler)
# print('Press Ctrl+C')
# signal.pause()