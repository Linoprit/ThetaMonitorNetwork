import logging
from tableIdTool.SensorId import SensorId as SensorId


# noinspection SpellCheckingInspection
class IdTableParser:
    """
    Converts the lines of the read file into the command-string.
    The file has to stick to the TableId-file-format:
    - Lines, starting with '#' are comments. Whitespaces before are allowed,
        inline-comments not.
    - Sensor-ID (64 bit), given as 8 hex numbers, comma seperated
        In case of Dallas 18b20, the id has to read out by the tools,
        in case of a BME280, the first four bytes are the station-ID, then
        following <sensor-type> on the LSB-side of the second four bytes.
    - min value, float [-12.5, 12.5]
        if measured value is below, RELAY will be switched on
    - max value, float [-12.5, 12.5]
        if measured value is above RELAY will be switched off
    - sensor type, uint
        refer to SensorIdTable.h: TEMP = 0, HUMIDITY = 1, PRESS = 2
    - relay, uint
        the correspondig relay that will switched. "0" means no relay
    - shortname (8 bytes)
        the name of the sensor, will be displayd at the stations lcd.
    example 18b20:
    0x28, 0xFF, 0x89, 0x0E, 0x02, 0x17, 0x03, 0x4A,	-12.5,  -12.7, 0, 0, "INNEN   "
    0x28, 0xFF, 0x45, 0xB1, 0xC2, 0x17, 0x01, 0xAD,	  2.0,   3.0,  0, 1, "WST_O   "
    example BME280, on Slave_01 (ID=256)
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, -12.5, -12.7,  0, 0, "LagerTmp"
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,  40.0,  80.0,  1, 0, "LagerHum"
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 846.0, 1100.0, 2, 0, "LagerPrs"
    """

    def __init__(self):
        """ file_conent is the read file, every line as one list-entry. """
        self.ID_LEN = 8
        self.POS_MIN = 8
        self.POS_MAX = 9
        self.POS_TYPE = 10
        self.POS_RELAY = 11
        self.POS_SHORTNAME = 12
        self._content = []

    def parse(self, file_content: list):
        line_count = -1
        for line in file_content:
            line_count += 1
            if line.startswith('#') or line == '\n' or not line :
                continue
            act_item = SensorId()
            splitted = line.split(",")
            sens_id = splitted[0: self.ID_LEN]
            if not act_item.set_sensor_ids_from_hex(sens_id):
                logging.getLogger().error(
                    "Length of sensor-id wrong in line " + str(line_count))
            sens_type = splitted[self.POS_TYPE]
            if not act_item.set_sensor_type(sens_type):
                logging.getLogger().error(
                    "unrecognized sensor-type [0, 2]/[temp, hum, pres] in line "
                    + str(line_count))
            min_val = float(splitted[self.POS_MIN])
            if not act_item.set_min_val(min_val, sens_type):
                logging.getLogger().error(
                    "min_value out of range in line " + str(line_count))
            max_val = float(splitted[self.POS_MAX])
            if not act_item.set_max_val(max_val, sens_type):
                logging.getLogger().error(
                    "max_value out of range in line " + str(line_count))
            relay = int(splitted[self.POS_RELAY])
            if not act_item.set_relay(relay):
                logging.getLogger().error(
                    "relay value out of range [0:255] in line " + str(line_count))
            shortname = splitted[self.POS_SHORTNAME].strip()
            if not act_item.set_shortname(shortname):
                logging.getLogger().error(
                    "shortname too long [max 8] in line " + str(line_count))

            self._content.append(act_item)
        logging.getLogger().info("ID Table parsed.")

    def get_content(self) -> list:
        return self._content

    def get_with_command(self, command: str):
        result = []
        for item in self._content:
            result.append(command + ' ' + item.to_str() + chr(13) )
        return result

    def get_content_strlist(self) -> str:
        result = []
        for item in self._content:
            item_str = item.to_str()
            result.append(item_str)
        return result

    def get_csv(self) -> str:
        prim = 0
        result = []
        for item in self._content:
            item_str = str(prim) + ', '
            item_str += item.to_csv()
            result.append(item_str)
            prim += 1
        return '\n'.join(result)
