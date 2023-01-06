import logging

import numpy as np
from enum import Enum
from serialIO.calcStm32Crc import CalcStm32Crc as Calc32


class SensorType(Enum):
    TEMP = 0
    HUMIDITY = 1
    PRESS = 2

    def __int__(self):
        return self.value


class SensorId:
    sensor_id = [] # list[np.uint8], len = 8
    sensor_id_hash = [] # list[np.uint32], len = 2
    min_val = np.float32(0.0)
    max_val = np.float32(0.0)
    sensor_type = SensorType.TEMP
    relay = np.uint8(0)
    shortname = "UNKNOWN " # len = 8

    def set_sensor_ids_from_hex(self, hex_list): # hex_list: eight hex strings
        if len(hex_list) != 8:            
            return False
        byte_list = [np.uint8(int(x, 16)) for x in hex_list]
        self.sensor_id = byte_list
        self.sensor_id_hash = self.calc_hash_from_str(hex_list)
        return True

    @staticmethod
    def calc_hash_from_str(hex_list) -> np.uint32: # hex_list: eight hex strings
        calc32 = Calc32()
        byte_list = [np.uint8(int(x, 16)) for x in hex_list]
        buffer32 = calc32.uint8_to_uint32_list(byte_list)
        result = calc32.hal_crc_calculate(buffer32)
        return result

    def set_min_val(self, min_val: float, sensor_type: str):
        """min / max values depend on the sensor type"""
        self.set_sensor_type(sensor_type)
        if not self.check_range_min_max(min_val, self.sensor_type):
            return False
        self.min_val = np.float32(min_val)
        return True

    def set_max_val(self, max_val: float, sensor_type: str):
        self.set_sensor_type(sensor_type)
        if not self.check_range_min_max(max_val, self.sensor_type):
            return False
        self.max_val = np.float32(max_val)
        return True

    def set_sensor_type(self, sensor_type: str):
        sens_str = sensor_type.strip().lower()
        if sens_str.startswith("temp") or sens_str == '0':
            self.sensor_type = SensorType.TEMP
        elif sens_str.startswith("hum") or sens_str == '1':
            self.sensor_type = SensorType.HUMIDITY
        elif sens_str.startswith("pres") or sens_str == '2':
            self.sensor_type = SensorType.PRESS
        else:
            return False
        return True

    def set_relay(self, relay_nr: int):
        if relay_nr > 255 or relay_nr < 0:
            return False
        self.relay = relay_nr
        return True

    def set_shortname(self, shortname: str):
        if shortname.startswith('\"') and shortname.endswith('\"'):
            self.shortname = shortname[1:-1]
        else:
            self.shortname = shortname
        if len(self.shortname) > 8:
            return False
        while len(self.shortname) < 8:
            self.shortname = self.shortname + ' '
        self.shortname = '\"' + self.shortname + '\"'
        return True

    def to_str(self):
        result = str(self.sensor_id_hash) + ' '
        result += f"{self.min_val:.1f}" + ' '
        result += f"{self.max_val:.1f}" + ' '
        result += str(int(self.sensor_type)) + ' '
        result += str(self.relay) + ' '
        result += self.shortname
        return result

    def to_csv(self):
        result = str(self.sensor_id_hash) + ','
        sid = ""
        for val in self.sensor_id:
            sid += "%0.2X" % val + ":"  # hex(val)[2:] + ":"
        result += sid[0:-1].strip() + ', '
        result += str(int(self.sensor_type)) + ', '
        result += f"{self.min_val:.1f}" + ', '
        result += f"{self.max_val:.1f}" + ', '
        result += str(self.relay) + ', '
        result += self.shortname
        return result

    @staticmethod
    def check_range_min_max(value: float, sensor_enum: SensorType):
        if sensor_enum == SensorType.TEMP:
            if value < -12.7 or value > 12.7:
                return False
        elif sensor_enum == SensorType.HUMIDITY:
            if value < 40.0 or value > 80.0:
                return False
        elif sensor_enum == SensorType.PRESS:
            if value < 846.0 or value > 1100.0:
                return False
        return True


