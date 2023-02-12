import unittest
from tests.unittest_setup import Unittest_setup
from daemon.DataBaseConnector import DataBaseConnector as DbConn

class Test_data_base_connector(unittest.TestCase):
    setup = Unittest_setup()
    settings = setup.get_standard_settings()

    def test_get_sens_type(self):
        sensorIdHash = 2394902346
        db = DbConn
        db.connect()
        result = db.get_sens_type(sensorIdHash)
        print (result)
        self.assertEqual(2, result)