import unittest

from daemon.DataBaseConnector import DataBaseConnector as DbConn
from tests.unittest_setup import Unittest_setup
from serialIO.streamDecoder import MeasurementType as MeasurementType
from datetime import timedelta, datetime


class TestDataBaseConnector(unittest.TestCase):
    setup = Unittest_setup()
    settings = setup.get_standard_settings()

    def test_get_sens_type(self):
        db = DbConn(self.settings)
        db.connect()

        sensorIdHash = 2394902346
        result = db.get_sens_type(sensorIdHash)
        self.assertEqual(2, result)

        sensorIdHash = 815
        result = db.get_sens_type(sensorIdHash)
        self.assertEqual(-1, result)

    def test_get_hashes_from_shortnames(self):
        shortnames = ["WST_O", "WrkstHum", "LGR_U"]
        one_shortnm = ["GGE_U"]
        unknown = ["bullshit"]
        hash_list = [ 734523852, 1294211458,3107815502]
        one_hash_lst = [3312979629]
        db = DbConn(self.settings)
        db.connect()
        result = db.get_hashes_from_shortnames(shortnames)
        self.assertEqual(result, hash_list)
        result = db.get_hashes_from_shortnames(one_shortnm)
        self.assertEqual(result, one_hash_lst)
        result = db.get_hashes_from_shortnames(unknown)
        self.assertEqual(result, [])

    def test_get_sensordata(self):
        shortnames = ["WST_O", "WrkstHum", "LGR_U"]
        db = DbConn(self.settings)
        db.connect()
        t_from = datetime.strptime("2022-11-21 18:04:11", "%Y-%m-%d %H:%M:%S")
        t_till = datetime.strptime("2022-11-21 18:09:11", "%Y-%m-%d %H:%M:%S")
        result = db.get_sensordata(shortnames, t_from, t_till)
        expect = \
            ((4, 3107815502, 0, 6.75, datetime(2022, 11, 21, 18, 4, 11)),
             (6, 1294211458, 0, 8.8125, datetime(2022, 11, 21, 18, 4, 14)),
             (11, 3107815502, 0, 6.75, datetime(2022, 11, 21, 18, 9, 11)),
             (13, 1294211458, 0, 8.8125, datetime(2022, 11, 21, 18, 9, 11)))
        self.assertEqual(result, expect)

    def test_update_sensordata_tbl(self):
        import datetime

        db = DbConn(self.settings)
        db.connect()
        self.clear_test_data_from_db(db)

        # 3338984827 = C7:04:DD:7B = 199:04:221:123
        bin_msg = [0, 1, 0, 0, 0, 123, 221, 4, 199, 0, 0, 147, 65, 162, 88, 9, 0, 162]
        msmnt = MeasurementType(bin_msg)
        db.update_sensordata(msmnt)

        shortnames = ["TESTTEMP"]
        t_till = datetime.datetime.today()
        t_from = datetime.datetime.today() - timedelta(hours=0, minutes=5)
        result = db.get_sensordata(shortnames, t_from, t_till)

        ts = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        expect = \
            (5000, 3338984827, 0, 18.375,
             datetime.datetime.strptime(ts, "%Y-%m-%d %H:%M:%S"))

        self.assertEqual(result[0][1], expect[1])
        self.assertEqual(result[0][2], expect[2])
        self.assertEqual(result[0][3], expect[3])
        self.assertEqual(result[0][4], expect[4])

    def clear_test_data_from_db(self, db: DbConn):
        sens_tbl = self.settings.get("deamon", "sens_tbl")
        command = "DELETE FROM {} WHERE AddressHash = '3338984827';"\
            .format(sens_tbl)
        db.exec_query(command)
