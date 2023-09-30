import unittest
import sys
from framework.settings import AppSettings
from datetime import datetime
from tests.unittest_setup import Unittest_setup


# noinspection SpellCheckingInspection
class TestAppSettings(unittest.TestCase):
    setup = Unittest_setup()
    settings = setup.get_standard_settings()

    # do not use 'settings' in the following tests!
    def testCommandLineOverwritesSetting(self):
        del sys.argv[1:]
        sys.argv += ["-v"]
        sut_config = AppSettings()
        self.assertEqual(sut_config.settings['common']['logLevel'], 'verbose')

        del sys.argv[1:]
        sys.argv += ["-d"]
        sut_config = AppSettings()
        self.assertEqual(sut_config.settings['common']['logLevel'], 'debug')

        del sys.argv[1:]
        sys.argv += ["-v", "-d"]
        sut_config = AppSettings()
        self.assertEqual(sut_config.settings['common']['logLevel'], 'debug')

        del sys.argv[1:]
        sut_config = AppSettings()
        self.assertEqual(sut_config.settings['common']['logLevel'], 'debug')

    def test_expand(self):
        base_path = self.settings.add_workdir("").as_posix()
        # $INPUT
        input_str = "$INPUT/someBlaBlah"
        self.assertEqual(base_path + "/input/someBlaBlah",
                         self.settings.expand(input_str))
        # $BASE_PATH
        base_path_str = "$BASE_PATH/anotherBlah"
        self.assertEqual(base_path + "/anotherBlah",
                         self.settings.expand(base_path_str))
        # $DATE
        current_date = datetime.today().strftime('%Y%m%d_%H%M%S')
        date_str = self.settings.expand('some $DATE contained')
        self.assertEqual('some ' + current_date + ' contained', date_str)
        # $OUTPUT
        output_str = "$OUTPUT/blahblah"
        self.assertEqual(base_path + "/output/blahblah",
                         self.settings.expand(output_str))
        # $TEMP
        temp_str = "$TEMP/someString"
        self.assertEqual(base_path + "/temp/someString",
                         self.settings.expand(temp_str))
        # $REFDATA
        refdata_str = "$REFDATA/someString"
        self.assertEqual(base_path + "/refdata/someString",
                         self.settings.expand(refdata_str))

    def test_get_date(self):
        current_date = datetime.today().strftime('%Y%m%d_%H%M%S')
        self.assertEqual(current_date,
                         self.settings.get_current_datetime())
        current_date_print = datetime.today().strftime('%Y/%m/%d %H:%M:%S')
        self.assertEqual(current_date_print,
                         self.settings.get_current_datetime('print'))

    def test_convenience(self):
        base_path = self.settings.add_workdir("").as_posix()
        self.assertEqual(self.settings.add_input_path("").as_posix(),
                         base_path + "/input")
        self.assertEqual(self.settings.add_output_path("").as_posix(),
                         base_path + "/output")
        self.assertEqual(self.settings.add_temp_path("").as_posix(),
                         base_path + "/temp")
        self.assertEqual(self.settings.add_refdata_path("").as_posix(),
                         base_path + "/refdata")
        self.settings.settings['common']['logLevel'] = 'debug'
        self.assertEqual(self.settings.get_loglevel(), 'debug')
