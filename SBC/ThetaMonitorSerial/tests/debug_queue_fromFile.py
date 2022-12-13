from queue import Queue
from threading import Thread
import time
import serial
from serialIO.streamDecoder import StreamDecoder as StreamDecoder
import numpy as np


class Worker(Thread):
    def __init__(self, serial_queue, filename):
        super(Worker, self).__init__()
        self._exit_is_requested = False
        self._file = open(filename, 'r')
        self._serial_queue = serial_queue

    def run(self):
        print(f'Thread-name: {self.name}\n')
        self.loop()

    def request_exit(self):
        self._exit_is_requested = True
        print("Exit is requested.")

    def get_data_from_file(self):
        data = []
        line = self._file.readline()
        for x in line.split(','):
            if x and (x != " \n"):
                data.append(int(x))
        return data

    def loop(self):
        while not self._exit_is_requested:
            data = self.get_data_from_file()
            if data:
                self._serial_queue.put(data, block=True, timeout=0.5)
            time.sleep(0.9)
        # some exit code
        self._file.close()
        print("Exiting")


def main():
    txt_queue = Queue()
    bin_queue = Queue()
    stream_decoder = StreamDecoder(txt_queue, bin_queue)
    serial_queue = Queue()
    worker = Worker(serial_queue, "./data/input/streamrecordMixed.txt")
    # worker = Worker(serial_queue, "./data/input/streamrecordBin.txt")
    worker.start()

    for i in range(30):
        print("pass " + str(i))
        for j in range(serial_queue.qsize() - 1):
            data = serial_queue.get(block=True, timeout=None)
            stream_decoder.proc_serial_stream(data)

            print_txt_queue(txt_queue)
            print_bin_queue(bin_queue)

        time.sleep(0.5)

    worker.request_exit()
    worker.join()


def print_txt_queue(txt_queue):
    while txt_queue.qsize() > 0:
        print(StreamDecoder.get_txt_queue_as_string(txt_queue))


def print_bin_queue(bin_queue):
    while bin_queue.qsize() > 0:
        result = bin_queue.get(block=True, timeout=None)
        print(" class: " + str(result.msgClass), end='')
        print(" station: " + str(result.stationId), end='')
        print(" siz: " + str(bin_queue.qsize()), end='')
        print(" tick: " + str(result.lastUpdateTick), end='')
        print(" hash: " + str(result.sensorIdHash), end='')
        print(" value: " + str(np.around(result.value, decimals=2)))
        print(" chksum: " + str(result.checksum), end='')
        bin_queue.task_done()
    print()


def do_stream_recording():
    f = open("./data/outfile.txt", "w")
    ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=0.5)
    print("start recording")
    for i in range(0, 30):
        data = ser.read(ser.inWaiting())
        if len(data) > 0:
            for one_byte in data:
                f.write(str(one_byte) + ", ")
            f.write("\n")
        print(str(i) + " ", end='')
        time.sleep(1.0)
    ser.close()
    print("\nfinished recording")


if __name__ == '__main__':
    main()
    # get_stream_recording()
