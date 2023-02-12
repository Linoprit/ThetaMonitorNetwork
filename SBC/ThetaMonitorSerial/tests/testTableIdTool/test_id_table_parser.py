import unittest
from tests.unittest_setup import Unittest_setup
import numpy as np
from framework.settings import AppSettings
from tableIdTool.IdTableParser import IdTableParser as IdTableParser
from tableIdTool.SensorId import SensorId as SensorId
from tableIdTool.SensorId import SensorType as SensorType
from framework.filehelper import Filehelper as Fh


class TestIdTableParser(unittest.TestCase):
    setup = Unittest_setup()
    settings = setup.get_standard_settings()

    def test_calc_hash(self):
        sens_id = SensorId()
        hex_list = ['0x28', '0xFF', '0x89', '0x0E', '0x02', '0x17', '0x03', '0x4A']
        result = sens_id.calc_hash_from_str(hex_list)
        self.assertEqual(result, 3103548024)

    def test_calc_hash_for_test_sensor(self):
        sens_id = SensorId()
        hex_list = ['0xFF', '0xFF', '0xFF', '0xFF', '0xFF', '0xFF', '0xFF', '0xFF']
        result = sens_id.calc_hash_from_str(hex_list)
        self.assertEqual(result, 3338984827)

    def test_set_shortname(self):
        sens_id = SensorId()
        sens_id.set_shortname("\"UNKNOWN \"")
        self.assertEqual('\"UNKNOWN \"', sens_id.shortname)
        sens_id.set_shortname("\"UNKNOWN\"")
        self.assertEqual('\"UNKNOWN \"', sens_id.shortname)
        sens_id.set_shortname("\"UNK\"")
        self.assertEqual('\"UNK     \"', sens_id.shortname)
        result = sens_id.set_shortname("\"UNKNOWNBlahBlahBlah\"")
        self.assertFalse(result)
        sens_id.set_shortname("UNKNOWN ")
        self.assertEqual('\"UNKNOWN \"', sens_id.shortname)
        sens_id.set_shortname("UNKNOW")
        self.assertEqual('\"UNKNOW  \"', sens_id.shortname)

    def test_set_relay(self):
        sens_id = SensorId()
        sens_id.set_relay(2)
        self.assertEqual(2, sens_id.relay)
        result = sens_id.set_relay(350)
        self.assertFalse(result)
        result = sens_id.set_relay(-2)
        self.assertFalse(result)

    def test_sensor_type(self):
        sens_id = SensorId()
        sens_id.set_sensor_type("temp")
        self.assertEqual(int(sens_id.sensor_type), 0)
        sens_id.set_sensor_type("hum")
        self.assertEqual(int(sens_id.sensor_type), 1)
        sens_id.set_sensor_type("pres")
        self.assertEqual(int(sens_id.sensor_type), 2)
        sens_id.set_sensor_type("temperature")
        self.assertEqual(int(sens_id.sensor_type), 0)
        sens_id.set_sensor_type("humidity")
        self.assertEqual(int(sens_id.sensor_type), 1)
        sens_id.set_sensor_type("pressure")
        self.assertEqual(int(sens_id.sensor_type), 2)
        sens_id.set_sensor_type("0")
        self.assertEqual(int(sens_id.sensor_type), 0)
        sens_id.set_sensor_type("1")
        self.assertEqual(int(sens_id.sensor_type), 1)
        sens_id.set_sensor_type("2")
        self.assertEqual(int(sens_id.sensor_type), 2)

    def test_set_max_val(self):
        sens_id = SensorId()
        sens_id.set_max_val(3.1, "temp")
        self.assertAlmostEqual(sens_id.max_val, 3.1, places=3)
        sens_id.set_max_val(-12.7, "temp")
        self.assertAlmostEqual(sens_id.max_val, -12.7, places=3)
        sens_id.set_max_val(12.7, "temp")
        self.assertAlmostEqual(sens_id.max_val, 12.7, places=3)
        result = sens_id.set_max_val(-13.0, "temp")
        self.assertFalse(result)
        result = sens_id.set_max_val(13, "temp")
        self.assertFalse(result)
        sens_id.set_max_val(50.2, "hum")
        self.assertAlmostEqual(sens_id.max_val, 50.2, places=3)
        sens_id.set_max_val(40.0, "hum")
        self.assertAlmostEqual(sens_id.max_val, 40.0, places=3)
        sens_id.set_max_val(80.0, "hum")
        self.assertAlmostEqual(sens_id.max_val, 80.0, places=3)
        result = sens_id.set_max_val(39.9, "hum")
        self.assertFalse(result)
        result = sens_id.set_max_val(80.1, "hum")
        self.assertFalse(result)
        sens_id.set_max_val(900.1, "pres")
        self.assertAlmostEqual(sens_id.max_val, 900.1, places=3)
        sens_id.set_max_val(846.0, "pres")
        self.assertAlmostEqual(sens_id.max_val, 846.0, places=3)
        sens_id.set_max_val(1100.0, "pres")
        self.assertAlmostEqual(sens_id.max_val, 1100.0, places=3)
        result = sens_id.set_max_val(845.0, "pres")
        self.assertFalse(result)
        result = sens_id.set_max_val(1101.0, "pres")
        self.assertFalse(result)

    def test_set_min_val(self):
        sens_id = SensorId()
        sens_id.set_min_val(3.1, "temp")
        self.assertAlmostEqual(sens_id.min_val, 3.1, places=3)
        sens_id.set_min_val(-12.7, "temp")
        self.assertAlmostEqual(sens_id.min_val, -12.7, places=3)
        sens_id.set_min_val(12.7, "temp")
        self.assertAlmostEqual(sens_id.min_val, 12.7, places=3)
        result = sens_id.set_min_val(-13.0, "temp")
        self.assertFalse(result)
        result = sens_id.set_min_val(13, "temp")
        self.assertFalse(result)
        sens_id.set_min_val(50.2, "hum")
        self.assertAlmostEqual(sens_id.min_val, 50.2, places=3)
        sens_id.set_min_val(40.0, "hum")
        self.assertAlmostEqual(sens_id.min_val, 40.0, places=3)
        sens_id.set_min_val(80.0, "hum")
        self.assertAlmostEqual(sens_id.min_val, 80.0, places=3)
        result = sens_id.set_min_val(39.9, "hum")
        self.assertFalse(result)
        result = sens_id.set_min_val(80.1, "hum")
        self.assertFalse(result)
        sens_id.set_min_val(900.1, "pres")
        self.assertAlmostEqual(sens_id.min_val, 900.1, places=3)
        sens_id.set_min_val(846.0, "pres")
        self.assertAlmostEqual(sens_id.min_val, 846.0, places=3)
        sens_id.set_min_val(1100.0, "pres")
        self.assertAlmostEqual(sens_id.min_val, 1100.0, places=3)
        result = sens_id.set_min_val(845.0, "pres")
        self.assertFalse(result)
        result = sens_id.set_min_val(1101.0, "pres")
        self.assertFalse(result)

    def test_set_sensor_id(self):
        sens_id = SensorId()
        id = ['0x28', '0x61', '0x4C', '0x46', '0x92', '0x0A', '0x02', '0xF7']
        sens_id.set_sensor_ids_from_hex(id)
        self.assertEqual(sens_id.sensor_id, [40, 97, 76, 70, 146, 10, 2, 247])
        self.assertEqual(sens_id.sensor_id_hash, np.uint32(1186060932))

    def test_parse_one_line(self):
        id_parser = IdTableParser()
        line = ["0x28, 0x61, 0x4C, 0x46, 0x92, 0x0A, 0x02, 0xF7,	-12.5,  -12.7, 0,  0,   \"UNKNOWN \""]
        id_parser.parse(line)
        result = id_parser.get_content_strlist()[0]
        self.assertEqual(result, "1186060932 -12.5 -12.7 0 0 \"UNKNOWN \"")

    def test_parse_file(self):
        id_parser = IdTableParser()
        id_file = Fh.read_file_to_list(self.settings.add_input_path('ID_Table.txt'))
        id_parser.parse(id_file)
        strlist = id_parser.get_content_strlist()
        ref = ['849700755 -12.5 -12.7 0 0 "InnenTmp"', '4006149412 40.0 80.0 1 0 "InnenHum"',
               '2394902346 846.0 1100.0 2 0 "InnenPrs"', '643027784 -12.5 -12.7 0 0 "LagerTmp"',
               '4198386175 40.0 80.0 1 0 "LagerHum"', '2588539793 846.0 1100.0 2 0 "LagerPrs"',
               '4155140987 -12.5 -12.7 0 0 "WrkstTmp"', '734523852 40.0 80.0 1 0 "WrkstHum"',
               '1269842850 846.0 1100.0 2 0 "WrkstPrs"', '3103548024 -12.5 -12.7 0 0 "INNEN   "',
               '1294211458 2.0 3.0 0 1 "WST_O   "', '3932845497 2.0 3.0 0 1 "WST_U   "',
               '3159888747 -12.5 -12.7 0 0 "Test 008"', '3312979629 -12.5 -12.7 0 0 "GGE_U   "',
               '1397168482 -12.5 -12.7 0 0 "ASN_O   "', '3822322055 -12.5 -12.7 0 0 "Test 007"',
               '4232230555 1.0 2.0 0 1 "LGR_O   "', '3107815502 1.0 2.0 0 1 "LGR_U   "',
               '196920976 -12.5 -12.7 0 0 "UNKNOWN "', '3044662693 -12.5 -12.7 0 0 "Test 003"',
               '3504230533 -12.5 -12.7 0 0 "Test 004"', '4058285955 -12.5 -12.7 0 0 "UNKNOWN "',
               '2802107143 -12.5 -12.7 0 0 "UNKNOWN "', '1186060932 -12.5 -12.7 0 0 "UNKNOWN "',
               '4130136082 -12.5 -12.7 0 0 "UNKNOWN "', '1431004260 -12.5 -12.7 0 0 "Test 005"',
               '3567186629 -12.5 -12.7 0 0 "Test 001"', '2575569828 -12.5 -12.7 0 0 "UNKNOWN "',
               '668176713 -12.5 -12.7 0 0 "UNKNOWN "', '1943981157 -12.5 -12.7 0 0 "Test 002"',
               '26376452 -12.5 -12.7 0 0 "Test 006"']
        self.assertEqual(strlist, ref)

    # Will always pass. It's just to get the file for importing it with phpadmin
    def test_write_csv(self):
        id_parser = IdTableParser()
        id_file = Fh.read_file_to_list(self.settings.add_input_path('ID_Table.txt'))
        id_parser.parse(id_file)
        Fh.dump_to_file(self.settings.add_temp_path('ID_Table.csv'), id_parser.get_csv())