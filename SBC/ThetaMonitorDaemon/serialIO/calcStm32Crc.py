import copy

import numpy as np
from struct import *


class CalcStm32Crc:
    """ Taken from x86-unittest-framework, file stm32f1xx_hal_crc.h """
    DEFAULT_CRC32_POLY = 0x04C11DB7
    DEFAULT_CRC_INITVALUE = 0xFFFFFFFF
    actCrc = DEFAULT_CRC_INITVALUE

    def resetCrc(self):
        self.actCrc = self.DEFAULT_CRC_INITVALUE

    def calcCrcUpdate(self, data: np.uint32):
        self.actCrc = self.actCrc ^ (data & self.DEFAULT_CRC_INITVALUE)

        for i in range(32):
            if self.actCrc & 0x80000000:
                self.actCrc = (self.actCrc << 1) ^ self.DEFAULT_CRC32_POLY
            else:
                self.actCrc <<= 1
            self.actCrc = self.actCrc & self.DEFAULT_CRC_INITVALUE
        return self.actCrc

    def hal_crc_accumulate(self, buffer: list):  # list[np.uint32]):
        for i in range(len(buffer)):
            self.calcCrcUpdate(buffer[i])
        return self.actCrc

    def hal_crc_calculate(self, buffer: list):  # list[np.uint32]):
        self.resetCrc()
        for i in range(len(buffer)):
            self.calcCrcUpdate(buffer[i])
        return self.actCrc

    @staticmethod
    def string_to_uint32_list(in_string):
        string_as_bytes = bytearray(in_string, 'utf-8')
        return CalcStm32Crc.uint8_to_uint32_list(string_as_bytes)

    @staticmethod
    def uint8_to_uint32_list(buffer: list) -> list:  # list[np.uint8]
        """ Takes in a list of uint8 values and interprets them into a uint32 list.
         This function takes respect to the little-endian thing.
         Example: [0x28, 0xFF, 0x89, 0x0E, 0x02, 0x17, 0x03, 0x4A]
         returns [243924776, 1241716482] = [0x0E89FF82, 0x4A031702]"""
        tmp_buffer = copy.copy(buffer)
        uint32len = CrcSocket.calcUint32Len(len(buffer))
        # pad string to a len, multiple of four
        uint8_diff_len = (uint32len * 4) - len(buffer)
        for i in range(uint8_diff_len):
            tmp_buffer.append(0)
        # reinterpret four bytes at a time to uint32
        uint32_buf = [0] * uint32len
        for i in range(0, uint32len):
            tmp = bytes(tmp_buffer[i*4:i*4+4])
            tmp_l = unpack("<L", tmp)
            uint32_buf[i] = tmp_l[0]
        return uint32_buf

    def check_crc_from_serial(self, serial_bin_msg: list):
        if len(serial_bin_msg) == 0:
            return False
        uint32_list = CalcStm32Crc.uint8_to_uint32_list(
            serial_bin_msg[0:-1])
        crc8 = self.hal_crc_calculate(uint32_list) & 0xFF
        return crc8 == serial_bin_msg[-1]


class CrcSocket:
    @staticmethod
    def calcChksum(data: list) -> np.uint8:  # list[np.uint8]
        calc_crc = CalcStm32Crc()
        """ Calls the CRC-Engine and returns a 8-bit CRC, which is
        simply the truncation of the 32-bit result.
        !! dataLen MUST BE DIVIDABLE BY 4 !!
        Params:
        data: pointer to array
        dataLen: size of the Array in bytes """
        if len(data) % 4 != 0:
            return 0
        crc = calc_crc.hal_crc_calculate(data)
        return crc & 0xFF

    @staticmethod
    def calcUint32Len(sizeInBytes: np.uint32) -> np.uint32:
        """ Calculates the size in uint32 multiples. Takes
        in the size in Bytes and returns
        how many uint32 values these are, if rounded up.
        i.E. char[3] = 3 bytes = 1 uint32; char[5] = 5 bytes = 2 uint32; """
        len32 = (sizeInBytes + 3) / 4
        return int(len32)

    @staticmethod
    def calcBufferedChkSum32(data: list) -> np.uint32: # list[np.ubyte]
        """ Takes in data of any size, buffers it in a uint32-array
        and pads the remainig bytes. The resulting array can
        directly be passed to the CRC-engine."""
        calc_crc = CalcStm32Crc()
        uint32len = CrcSocket.calcUint32Len(len(data))
        data32 = [0] * uint32len
        for i in range(len(data)):
            data32[i] = data[i]
        return calc_crc.calcChksum32(data32)
