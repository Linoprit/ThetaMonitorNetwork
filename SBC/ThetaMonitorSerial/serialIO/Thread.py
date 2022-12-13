from multiprocessing import Queue
from threading import Thread
import time
import serial
from serialIO.streamDecoder import StreamDecoder as StreamDecoder


class Worker(Thread):
    def __init__(self, txt_queue, bin_queue, port, baud, timeout):
        super(Worker, self).__init__()
        self.stream_decoder = StreamDecoder(txt_queue, bin_queue)
        self._exit_is_requested = False
        self.serial = serial.Serial(
            port=port, baudrate=baud, timeout=timeout)

    def run(self):
        print(f'Thread-name: {self.name}\n')
        self.loop()

    def request_exit(self):
        self._exit_is_requested = True
        print("Exit is requested.")

    def loop(self):
        # f = open("./data/outfile.txt", "w")
        while not self._exit_is_requested:
            data = self.serial.read(self.serial.inWaiting())
            if len(data) > 0:
                # for one_byte in data:
                #   print(int.from_bytes(one_byte, byteorder='big', signed=False))
                #   f.write(str(one_byte) + ", ")
                # f.write("\n")
                self.stream_decoder.proc_serial_stream(data)
            time.sleep(0.1)
        # some exit code
        # f.close()
        self.serial.close()
        print("Exiting")


def main():
    text_queue = Queue()
    bin_queue = Queue()
    worker = Worker(text_queue, bin_queue, '/dev/ttyUSB0', 115200, 0.5)
    worker.start()

    for i in range(30):
        if not text_queue.empty():
            print(StreamDecoder.get_txt_queue_as_string(text_queue))

        # print("got " + str(bin_queue.qsize()))
        while not bin_queue.empty():
            bin_result_msg = bin_queue.get(block=True, timeout=0.1)
            # print("mn: ", end='')
            # print(bin_result_msg)
            result = StreamDecoder.get_bin_msg_decoded(bin_result_msg)
            # print("siz: " + str(bin_queue.qsize()), end='')
            print(" chksum: " + str(result.checksum), end='')
            # print(" tick: " + str(result.lastUpdateTick), end='')
            # print(" class: " + str(result.msgClass), end='')
            # print(" hash: " + str(result.sensorIdHash), end='')
            # print(" station: " + str(result.stationId), end='')
            # print(" value: " + str(np.around(result.value,decimals=2)))

        time.sleep(1.4)

    worker.request_exit()
    worker.join()

####
# https://pyserial.readthedocs.io/en/latest/pyserial_api.html
# https://stackoverflow.com/questions/17553543/pyserial-non-blocking-read-loop
# 
# connected = False
# port = 'COM4'
# baud = 9600
# serial_port = serial.Serial(port, baud, timeout=1.0)
# # serial_port.write_timeout()
# def handle_data(data):
#     print(data)
#
# def read_from_port(ser):
#     while not connected:
#         #serin = ser.read()
#         connected = True
#         while True:
#            print("test")
#            reading = ser.readline().decode()
#            handle_data(reading)
# thread = threading.Thread(target=read_from_port, args=(serial_port,))
# thread.start()
