import framework.settings
from pathlib import Path
import json
import logging


class HtmlConfig:
    def __init__(self, settings_in: framework.settings.AppSettings):
        self.settings = settings_in
        self.page_conf = None
        self.plot_conf = None
        self.content_conf = None
        self.conf_file_ok = self.open_htmlconf_file()

    def open_htmlconf_file(self):
        html_conf_file = self.settings.expand(
            self.settings.get("daemon", "html_config"))
        if not Path(html_conf_file).is_file():
            logging.getLogger().error(
                "No Html-config-file found. Will not generate plots.")
            return False
        with open(html_conf_file, 'r') as html_config:
            configs = json.load(html_config)
            for conf in configs:
                if "plot_config" in conf:
                    self.plot_conf = conf["plot_config"]
                if "page_config" in conf:
                    self.page_conf = conf["page_config"]
                if "content_config" in conf:
                    self.content_conf = conf["content_config"]
        return True

    def get_plot_config(self):
        if self.conf_file_ok:
            return self.plot_conf
        return None

    def get_page_config(self):
        if self.conf_file_ok:
            return self.page_conf
        return None

    def get_content_config(self):
        if self.conf_file_ok:
            return self.content_conf
        return None
