import unittest
from framework.settings import AppSettings
from struct import *

from serialIO.calcStm32Crc import CalcStm32Crc as CalcStm32Crc, CrcSocket
from tests.unittest_setup import Unittest_setup
import numpy as np

# print("{0:b}".format(0x4C))


class TestAppSettings(unittest.TestCase):
    setup = Unittest_setup()
    settings = setup.get_standard_settings()

    def test_CrcSocket_algorithm_Test_1(self):
        data: np.ubyte = 0xC1
        poly: np.ubyte = 0xCB
        initialCrc: np.ubyte = 0xFF

        # Result in single steps
        # 0x3E 0x7C 0xF8 0x3B 0x76 0xEC 0x13 0x26 0x4C
        actCrc: np.ubyte = initialCrc ^ data
        for i in range(8):
            if actCrc & 0x80:
                actCrc = (actCrc << 1) ^ poly
            else:
                actCrc <<= 1
            actCrc = actCrc & 0xff
        self.assertEqual(0x4C, actCrc)

    def test_CrcSocket_algorithm_Test_2(self):
        calc_crc = CalcStm32Crc()
        data: np.uint = 0x11223344
        calc_crc.resetCrc()
        crc: np.uint = calc_crc.calcCrcUpdate(data)
        self.assertEqual(0xB14257CC, crc)

    def test_CrcSocket_code_1(self):
        calc_crc = CalcStm32Crc()
        test_str = "e2dumpjournal"
        uint32_buff = calc_crc.string_to_uint32_list(test_str)
        crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
        self.assertEqual(1884395996, crc)

    def test_CrcSocket_code_2(self):
        calc_crc = CalcStm32Crc()
        test_str = "e2init"
        uint32_buff = calc_crc.string_to_uint32_list(test_str)
        crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
        self.assertEqual(3743823511, crc)

    def test_CrcSocket_code_3(self):
        calc_crc = CalcStm32Crc()
        test_str = "e2dumpjournalmap"
        uint32_buff = calc_crc.string_to_uint32_list(test_str)
        crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
        self.assertEqual(2362017329, crc)

    def test_CrcSocket_code_4(self):
        calc_crc = CalcStm32Crc()
        test_str = "e2erase"
        uint32_buff = calc_crc.string_to_uint32_list(test_str)
        crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
        self.assertEqual(3704805521, crc)

    def test_CrcSocket_code_5(self):
        calc_crc = CalcStm32Crc()
        test_str = "e2dumpcalib"
        uint32_buff = calc_crc.string_to_uint32_list(test_str)
        crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
        self.assertEqual(2821683926, crc)

    def test_CrcSocket_code_6(self):
        calc_crc = CalcStm32Crc()
        test_str = "e2dumpmem"
        uint32_buff = calc_crc.string_to_uint32_list(test_str)
        crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
        self.assertEqual(1704472323, crc)

    def test_CrcSocket_code_7(self):
        calc_crc = CalcStm32Crc()
        test_str = "e2dumpstart"
        uint32_buff = calc_crc.string_to_uint32_list(test_str)
        crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
        self.assertEqual(302299815, crc)
