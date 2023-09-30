import argparse
import configparser
import logging
import pathlib
import sys
from datetime import datetime
import logging.handlers


# noinspection SpellCheckingInspection
class AppSettings:
    def __init__(self):
        self.args = self._command_line_init()

        self.workdir = self.__check_workdir(self.args)
        self.__check_workdir_structure(self.workdir)

        if self.args.settingsFile:
            self.settings_file = pathlib.Path(self.workdir, self.args.settingsFile)
        else:
            self.settings_file = pathlib.Path(self.workdir, "settings.ini")

        self.settings = self.__create_or_read_settings(self.settings_file)
        self.__overlay_settings(self.args)

        self.__setup_logger(pathlib.Path(self.workdir, "logfile.log"), self.get_loglevel())
        self.set_name_and_version()  # set the defaults

    def __create_or_read_settings(self, settings_file: pathlib.Path) -> configparser.ConfigParser:
        config = configparser.ConfigParser()
        if not self.settings_file.exists():
            config.add_section('common')
            config['common']['comment'] = "Automatically created with defaults."
            config['common']['logLevel'] = "info"
            with open(settings_file, 'w') as file:
                config.write(file)
        else:
            config.read(settings_file, 'UTF-8')

        return config

    def __check_workdir_structure(self, workdir: pathlib.Path):
        # !! ref path is NOT checked, as it is intended for tests only !!
        input_path = pathlib.Path(workdir, "input")
        out_path = pathlib.Path(workdir, "output")
        temp_path = pathlib.Path(workdir, "temp")
        if not input_path.exists():
            input_path.mkdir(parents=True)
        if not out_path.exists():
            out_path.mkdir(parents=True)
        if not temp_path.exists():
            temp_path.mkdir(parents=True)

    def __check_workdir(self, args) -> pathlib.Path:
        if args.workdir:
            workdir = pathlib.Path(args.workdir)
            if not workdir.exists():
                workdir.mkdir(parents=True)
        else:
            workdir = pathlib.Path("./data")  # standard
            if not workdir.exists():
                workdir = pathlib.Path("c:/temp/data")  # simple mode
        return workdir

    def _command_line_init(self):
        self.arg_parser = argparse.ArgumentParser()
        self.arg_parser.add_argument(
            '-w', '--workdir', help="Path to the working directory, named 'data', where "
                                    "the subdirs 'input', 'output' and 'temp' reside. "
                                    "Will be created, if not in existence.")
        self.arg_parser.add_argument(
            '-s', '--settingsFile', help="settings file to use, without path."
                                         "It must be located inside the workdir "
                                         "and it will be created, if it doesn't exist. ")
        self.arg_parser.add_argument(
            '-v', '--verbose', action='store_true',
            help='print verbose messages')
        self.arg_parser.add_argument(
            '-d', '--debug', action='store_true',
            help='print verbose and debug messages')
        self.arg_parser.add_argument(
            '--version', action='store_true', help="Shows programm version and exits")
        return self.arg_parser.parse_args()

    @staticmethod
    def __setup_logger(filename, log_level_str):
        log_level = logging.WARNING
        if log_level_str.lower() == "debug":
            log_level = logging.DEBUG
        elif log_level_str.lower() == "verbose":
            log_level = logging.INFO

        logging.basicConfig(format='%(asctime)s [%(levelname)-7.7s] %(message)s',
                            datefmt='%Y-%m-%d %H:%M:%S',
                            level=log_level)
        log_formatter = logging.Formatter("%(asctime)s [%(levelname)-7.7s] %(message)s")

        file_handler = logging.handlers.TimedRotatingFileHandler(
            filename, when='midnight', backupCount=5)
        file_handler.setFormatter(log_formatter)
        logging.getLogger().addHandler(file_handler)

        # console_handler = logging.StreamHandler(sys.stdout)
        # console_handler.setFormatter(log_formatter)
        # logging.getLogger().addHandler(console_handler)

    # overwrite the settings from ini-file with commandline-args, if present
    def __overlay_settings(self, args):
        if args.verbose:
            self.settings['common']['logLevel'] = 'verbose'
        if args.debug:
            self.settings['common']['logLevel'] = 'debug'

    def print_status(self):
        if self.args.version:
            print(self.get_appname() + " " + self.get_version())
            sys.exit(0)
        logging.info(self.get_appname() + " " + self.get_version())
        logging.info("Settings comment: " + self.settings['common']['comment'])
        logging.info("working dir is: '" + self.add_workdir("").as_posix() + "'")
        logging.info("logLevel is: '" + self.get_loglevel() + "'")

    def get(self, section: str, value: str, uppercase: bool = False) -> str:
        if self.settings.has_option(section, value):
            result = self.settings[section][value]
            if uppercase:
                return result.upper()
            else:
                return result
        else:
            return None

    def get_as_int(self, section: str, value: str) -> int:
        result = self.get(section, value)
        if result is None:
            return None
        return int(result)

    def get_as_path(self, section: str, value: str) -> pathlib.Path:
        result = self.get(section, value)
        if result is None:
            return None
        return pathlib.Path(self.expand(result))

    def set(self, section: str, name: str, value: str):
        self.settings[section][name] = value

    def get_settings(self):
        return self.settings

    def get_version(self):
        return self.settings['common']['app_version']

    def get_appname(self):
        return self.settings['common']['app_name']

    def set_name_and_version(self, app_name='unknownApp', app_version='v000.000.000'):
        self.settings['common']['app_name'] = app_name
        self.settings['common']['app_version'] = app_version

    def expand(self, config_string: str):
        predefined_symbols = {"$INPUT": "input", "$OUTPUT": "output",
                              "$TEMP": "temp", "$REFDATA": "refdata"}
        predefined_symbols.update({"$BASE_PATH": self.add_workdir("").as_posix()})
        predefined_symbols.update({"$DATE": self.get_current_datetime()})
        predefined_symbols.update({"$INPUT": self.add_input_path("").as_posix()})
        predefined_symbols.update({"$OUTPUT": self.add_output_path("").as_posix()})
        predefined_symbols.update({"$TEMP": self.add_temp_path("").as_posix()})
        predefined_symbols.update({"$REFDATA": self.add_refdata_path("").as_posix()})

        for symbol, replacement in predefined_symbols.items():
            config_string = config_string.replace(symbol, replacement)

        return config_string

    @staticmethod
    def get_current_datetime(purpose='filename') -> str:
        """
        Returns a string with current date and time.
        :param: determines the shape of the retured string.
            'filename' -> 20220919_073418
            'print' -> 2022/09/19 07:34:18
        """
        if purpose == 'filename':
            date = datetime.today().strftime('%Y%m%d_%H%M%S')
        elif purpose == 'print':
            date = datetime.today().strftime('%Y/%m/%d %H:%M:%S')
        return date

    # convenience functions, for the labels in the 'common' section
    def add_workdir(self, fileorpath="") -> pathlib.Path:
        return pathlib.Path(self.workdir, fileorpath)

    def add_input_path(self, fileorpath="") -> pathlib.Path:
        return pathlib.Path(self.workdir, "input", fileorpath)

    def add_output_path(self, fileorpath="") -> pathlib.Path:
        return pathlib.Path(self.workdir, "output", fileorpath)

    def add_temp_path(self, fileorpath="") -> pathlib.Path:
        return pathlib.Path(self.workdir, "temp", fileorpath)

    def add_refdata_path(self, fileorpath="") -> pathlib.Path:
        return pathlib.Path(self.workdir, "refdata", fileorpath)

    def get_loglevel(self):
        return self.expand(self.settings['common']['logLevel'])
