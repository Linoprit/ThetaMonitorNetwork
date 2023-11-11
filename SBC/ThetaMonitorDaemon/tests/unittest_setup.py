import framework.settings
import sys


class Unittest_setup:
    def __init__(self):
        del sys.argv[1:]
        # sys.argv += ["-ssettings.ini"]
        sys.argv += ["-w./data"]
        self.settings = framework.settings.AppSettings()

    def get_standard_settings(self) -> framework.settings.AppSettings:
        return self.settings

