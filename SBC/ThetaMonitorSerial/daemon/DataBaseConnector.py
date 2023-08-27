import logging
import math

import pymysql.connections
from pymysql import Error
from datetime import datetime, timedelta
import framework.settings
from serialIO.streamDecoder import MeasurementType as MeasurementType
from serialIO.streamDecoder import RadioStatisticsType as RadioStatisticsType


class DataBaseConnector:
    def __init__(self, settings_in: framework.settings.AppSettings):
        self.settings = settings_in
        self.db_name = self.settings.get("daemon", "db_name")
        self.sens_tbl = self.settings.get("daemon", "sens_tbl")
        self.senstype_tbl = self.settings.get("daemon", "senstype_tbl")
        self.sensid_tbl = self.settings.get("daemon", "sensid_tbl")
        self.stationdata_tbl = self.settings.get("daemon", "station_tbl")
        self.stationIds_tbl = self.settings.get("daemon", "stationId_tbl")
        self.user_id = self.settings.get("daemon", "user_id")
        self.host = self.settings.get("daemon", "host")
        self.passwd = self.settings.get("daemon", "entry")
        self.db_conn = None

    def connect(self):
        try:
            self.db_conn = pymysql.connect(host=self.host,
                                           database=self.db_name,
                                           user=self.user_id,
                                           password=self.passwd)

            db_info = self.db_conn.get_server_info()
            logging.info("Connected to MySQL Server version: " + db_info)
            cursor = self.db_conn.cursor()
            cursor.execute("select database();")
            record = cursor.fetchone()
            logging.info("You're connected to database: " + record[0])

        except Error as e:
            logging.info("Error while connecting to MySQL", e)
            self.close()

    def is_connect(self):
        try:
            self.db_conn.ping(reconnect=False)
        except Error:
            return False
        return True

    def reconnect(self):
        self.db_conn.ping(reconnect=True)
        logging.info("MySQL reconnected")

    def close(self):
        if self.is_connect():
            self.db_conn.close()
            self.db_conn.cursor().close()
            logging.info("MySQL connection is closed")

    def update_sensordata(self, measurement: MeasurementType):
        #  	PrimKey AddressHash SensorType Measurement  	TimeStamp
        address_hash = str(measurement.sensorIdHash)
        if math.isnan(measurement.value):
            logging.getLogger().error("Tried to write NaN-value to DB: {}".format(address_hash))
            return -1
        value = f"{measurement.value:.3f}"
        last_update_tick = str(measurement.lastUpdateTick)
        sensor_type = self.get_sens_type(address_hash)
        command = "INSERT INTO {} (AddressHash, SensorType, Measurement, LastUpdateTick) " \
                  "VALUES ({}, {}, {}, {});" \
            .format(self.sens_tbl, address_hash, sensor_type, value, last_update_tick)
        result = self.exec_query(command)
        if result < 0:
            logging.getLogger().error("Update sensordata went wrong, query='{}'"
                                      .format(command))
            return -1
        return 0

    def get_sensordata(self, shortnames: list, t_from: datetime, t_till: datetime):
        hashes = self.get_hashes_from_shortnames(shortnames)
        hashesFlat = (', '.join('"' + str(item) + '"' for item in hashes))
        from_str = t_from.strftime("%Y-%m-%d %H:%M:%S")
        till_str = t_till.strftime("%Y-%m-%d %H:%M:%S")
        command = "SELECT * FROM {} WHERE `AddressHash` IN({}) and " \
                  "`TimeStamp` BETWEEN '{}' AND '{}'" \
            .format(self.sens_tbl, hashesFlat, from_str, till_str)
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        return cursor.fetchall()

    def get_single_sensordata(self, shortname: str, t_from: datetime, t_till: datetime):
        sens_hash = self.get_hashes_from_shortnames([shortname])
        if len(sens_hash) == 0:
            logging.getLogger().info("No hash found for " + shortname)
            return
        hashFlat = ('"' + str(sens_hash[0]) + '"')
        from_str = self.datetime_to_str(t_from)
        till_str = self.datetime_to_str(t_till)
        # PrimKey 	AddressHash 	SensorType 	Measurement 	TimeStamp
        command = ("SELECT TimeStamp, Measurement FROM {} WHERE `AddressHash` IN({}) "
                   "and `TimeStamp` BETWEEN '{}' AND '{}'"
                   .format(self.sens_tbl, hashFlat, from_str, till_str))
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        return cursor.fetchall()

    def get_last_sensordata(self):
        # from_str = "2023-08-06 19:24:09"
        # till_str = "2023-08-06 19:30:09"
        from_str = self.datetime_to_str(datetime.today())
        till_str = self.datetime_to_str(datetime.today() - timedelta(minutes=6))
        vals_str = "AddressHash, SensorType, Measurement, LastUpdateTick, TimeStamp"
        command = ("SELECT {} FROM {} WHERE `TimeStamp` BETWEEN '{}' AND '{}'"
                   .format(vals_str, self.sens_tbl, from_str, till_str))
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        return cursor.fetchall()

    def get_station_stats(self, station_id, data_names: [], t_from: datetime, t_till: datetime):
        from_str = self.datetime_to_str(t_from)
        till_str = self.datetime_to_str(t_till)
        data_str = ", ".join(data_names)
        command = ("SELECT TimeStamp, {} FROM {} WHERE `Station_ID` = '{}' AND"
                   "`TimeStamp` BETWEEN '{}' AND '{}' "
                   .format(data_str, self.stationdata_tbl, station_id, from_str, till_str))
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        return cursor.fetchall()

    def get_statistikdata(self, t_from: datetime, t_till: datetime):
        from_str = self.datetime_to_str(t_from)
        till_str = self.datetime_to_str(t_till)
        station_data = []
        station_ids = self.get_station_ids_from_statistik(from_str, till_str)
        for stat_id in station_ids:
            command = ("SELECT Station_ID, Lost_Pkgs, Valid_Sensors FROM {} "
                       "WHERE `Station_ID` = '{}' AND"
                       "`TimeStamp` BETWEEN '{}' AND '{}' "
                       .format(self.stationdata_tbl, stat_id, from_str, till_str))
            cursor = self.db_conn.cursor()
            cursor.execute(command)
            station_data.append(cursor.fetchall())
        return station_data

    def get_station_ids_from_statistik(self, from_str: str, till_str: str):
        command = ("SELECT Station_ID FROM {} WHERE `TimeStamp` BETWEEN '{}' AND '{}' "
                   "GROUP BY Station_ID"
                   .format(self.stationdata_tbl, from_str, till_str))
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        station_ids = cursor.fetchall()
        result = []
        for i in range(len(station_ids)):
            result.append(station_ids[i][0])
        return result

    def get_hashes_from_shortnames(self, shortnames: list):  # string-list
        newShortnames = []
        for name in shortnames:  # pad strings to eight chars
            newName = name.ljust(8)
            newShortnames.append(newName)
        shortNamesFlat = (', '.join('"' + item + '"' for item in newShortnames))
        command = "SELECT `AddressHash` FROM {} WHERE `ShortName` IN({})" \
            .format(self.sensid_tbl, shortNamesFlat)
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        data = cursor.fetchall()
        result = []
        for item in data:
            result.append(item[0])
        return result

    def get_sens_type(self, sensorIdHash):
        addrHash = str(sensorIdHash)
        command = "SELECT SensorType FROM `" + self.sensid_tbl + \
                  "` WHERE `AddressHash` = " + addrHash
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        result = cursor.fetchone()
        if result is None:
            return -1
        return result[0]

    def exec_query(self, query):
        cursor = self.db_conn.cursor()
        try:
            cursor.execute(query)
            self.db_conn.commit()
        except pymysql.err.ProgrammingError:
            return -1
        return 0

    def update_stationdata_tbl(self, stats: RadioStatisticsType):
        # PrimKey 	Station_ID 	Lost_Pkgs 	Valid_Sensors
        # rxBuff_Overflows 	Last_Update_Tick
        stationId = str(stats.stationId)
        lost_pkgs = str(stats.lostPkgs)
        valid_sensors = str(stats.validSensors)
        rxBuff_ovfl = str(stats.rxBufferOverflows)
        lastUpdateTick = str(stats.lastUpdateTick)
        command = "INSERT INTO {} (Station_ID, Lost_Pkgs, Valid_Sensors," \
                  " rxBuff_Overflows, Last_Update_Tick) " \
                  "VALUES ({}, {}, {}, {}, {});" \
            .format(self.stationdata_tbl, stationId, lost_pkgs,
                    valid_sensors, rxBuff_ovfl, lastUpdateTick)
        result = self.exec_query(command)
        if result < 0:
            logging.getLogger().error("Update stationdata went wrong, query='{}'"
                                      .format(command))
            return -1
        return 0

    def get_id_from_station_name(self, station_names: list):  # string-list
        # PrimKey 	StationId 	Shortname 	Description
        shortNamesFlat = (', '.join('"' + item + '"' for item in station_names))
        command = "SELECT `StationId` FROM {} WHERE `ShortName` IN({})" \
            .format(self.stationIds_tbl, shortNamesFlat)
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        data = cursor.fetchall()
        result = []
        for item in data:
            result.append(item[0])
        return result

    def get_stationdata(self, station_name: list, t_from: datetime, t_till: datetime):
        station_ids = self.get_id_from_station_name(station_name)
        station_ids_Flat = (', '.join('"' + str(item) + '"' for item in station_ids))
        from_str = t_from.strftime("%Y-%m-%d %H:%M:%S")
        till_str = t_till.strftime("%Y-%m-%d %H:%M:%S")
        command = "SELECT * FROM {} WHERE `AddressHash` IN({}) and " \
                  "`TimeStamp` BETWEEN '{}' AND '{}'" \
            .format(self.sens_tbl, station_ids_Flat, from_str, till_str)
        cursor = self.db_conn.cursor()
        cursor.execute(command)
        return cursor.fetchall()

    @staticmethod
    def str_to_datetime(date_str: str):
        format_str = "%Y-%m-%d %H:%M:%S"
        return datetime.strptime(date_str, format_str)

    @staticmethod
    def datetime_to_str(t_datetime: datetime):
        return t_datetime.strftime("%Y-%m-%d %H:%M:%S")

    def update_id_table(self):
        # ToDo implement update_id_table
        pass

    def update_sensortypes_tbl(self):
        # ToDo implement update_sensortypes_tbl
        pass
