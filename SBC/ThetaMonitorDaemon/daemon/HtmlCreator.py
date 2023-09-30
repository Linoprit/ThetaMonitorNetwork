import dominate
import os
from datetime import datetime
from dominate.tags import *
from dominate.util import raw
from pathlib import Path
import framework.settings
import shutil
from daemon.HtmlConfigStructure import HtmlConfig
from daemon.DataBaseConnector import DataBaseConnector as Dbcon


class HtmlCreator:
    def __init__(self,
                 settings_in: framework.settings.AppSettings,
                 db_con: Dbcon,
                 html_conf: HtmlConfig):
        self.db = db_con
        self.settings = settings_in
        self.html_config = html_conf
        html_outdir = self.settings.expand(
            self.settings.get("daemon", "html_outdir"))
        self.test_file_and_copy(
            "w3.css", html_outdir, self.settings.add_input_path("html"))
        self.test_file_and_copy(
            "w3-theme-black.css", html_outdir, self.settings.add_input_path("html"))

    def create_html_pages(self):
        # Todo add table with recent values of all sensors
        # Todo do we need to add the logfile?
        content_configs = self.html_config.get_content_config()
        for conf in content_configs:
            conf_name = conf["name"]
            filename = conf["filename"]
            with dominate.document(title="Theta Monitor " + conf_name) as doc:
                button_name = "testbtn"
                self.create_head()
                self.create_header(conf_name)
                self.create_buttons(button_name)
                self.create_menu_div(conf_name)
                self.create_divisions(conf_name)
                self.create_footer()
                self.add_script(button_name)
            self.write_htmldoc(doc, filename)

    @staticmethod
    def create_head():
        with head() as result:
            meta(charset="UTF-8")
            meta(name="viewport", content="width=device-width, initial-scale=1")
            link(rel='stylesheet', href='w3.css')
            link(rel='stylesheet', href='w3-theme-black.css')
            style("""\
                .owntable{border-collapse:collapse; border-spacing:0; display: fixed;}
                table.center { margin-left: auto; margin-right: auto; }
                img {max-width: 900px;}
                img.center { display: block; margin-left: auto; margin-right: auto; width: 100%; }
            """)
        return result

    @staticmethod
    def create_header(scope_str):
        header_str = "Theta Monitor " + scope_str
        with (div(id='header', cls='w3-container w3-theme w3-padding-8 w3-center')
              as result):
            comment('Header')
            h4(header_str)
        return result

    @staticmethod
    def create_footer():
        with footer(cls='w3-container w3-theme-dark') as result:
            t_now = datetime.today().strftime("%Y-%m-%d %H:%M:%S")
            small('Updated: ' + t_now)
            small('Theta Monitor Network (2023), Powered by')
            a('w3.css', href='https://www.w3schools.com/w3css/default.asp', target='_blank')
        return result

    def write_htmldoc(self, doc: dominate.document, filename):
        html_outdir = self.settings.expand(
            self.settings.get("daemon", "html_outdir"))
        outfile = os.path.join(html_outdir, filename)
        with open(outfile, 'w') as f:
            f.write(doc.render())

    def create_buttons(self, button_name):
        result = div(cls='w3-bar w3-orange')
        cls_str = 'w3-bar-item w3-button ' + button_name
        divs = self.html_config.get_page_config()["divisions"]
        for one_div in divs:
            div_name = one_div["div_name"]
            name = one_div["name"]
            on_click_str = "openCity(event,\'{}\')".format(div_name)
            result.add(button(name, cls=cls_str, onclick=on_click_str))
        return result

    def create_menu_div(self, content_config_name):
        with div(cls='w3-bar w3-orange') as result:
            content_configs = self.html_config.get_content_config()
            for conf in content_configs:
                cls_str = 'w3-bar-item w3-button w3-right'
                if content_config_name == conf["name"]:
                    cls_str = cls_str + ' w3-dark-grey'
                a(conf["name"], href=conf["filename"], cls=cls_str)
        return result

    def create_divisions(self, content_config_name):
        cls_str = 'w3-border w3-padding center'
        divs = self.html_config.get_page_config()["divisions"]
        result = None
        for one_div in divs:
            div_name = one_div["div_name"]
            result = div(id=div_name, cls='ww3-container city w3-animate-opacity w3-center')
            result.add(comment(div_name))
            result.add(hr())
            plots = one_div["plots"]
            for plot in plots:
                if plot == "$LAST_SENSOR_DATA_TABLE":
                    if content_config_name == "24h":
                        self.add_last_values_tbl(result)
                    continue
                filename = plot + "_" + content_config_name + ".png"
                path = os.path.join("plots", filename)
                result.add(img(src=path, cls=cls_str, alt=plot))
                result.add(br())
            result.add(hr())
        return result

    def add_last_values_tbl(self, division: div):
        db_result = self.db.get_last_sensordata()
        division.add(hr())
        with table(cls='owntable w3-bordered center', style="width:40%") as tbl:
            with tr(cls='w3-theme'):
                th("TimeStamp")
                th("AddressHash")
                th("SensorType")
                th("Measurement")
                th("LastUpdateTick")
            for sens_data in db_result:
                with tr(cls='w3-white'):
                    td(Dbcon.datetime_to_str(sens_data[4]))  # TimeStamp
                    td(sens_data[0])  # AddressHash
                    td(sens_data[1])  # SensorType
                    td(sens_data[2])  # Measurement
                    td(sens_data[3])  # LastUpdateTick
        division.add(tbl)

    @staticmethod
    def add_script(button_name):
        script_str = """
            // Tabs
            function openCity(evt, cityName) {
              var i;
              var x = document.getElementsByClassName("city");
              for (i = 0; i < x.length; i++) {
                x[i].style.display = "none";
              }
              var activebtn = document.getElementsByClassName("testbtn");
              for (i = 0; i < x.length; i++) {
                activebtn[i].className = activebtn[i].className.replace(" w3-dark-grey", "");
              }
              document.getElementById(cityName).style.display = "block";
              evt.currentTarget.className += " w3-dark-grey";
            }
            var mybtn = document.getElementsByClassName("testbtn")[0];
            mybtn.click();
        """
        return script(raw(script_str.replace("testbtn", button_name)))

    @staticmethod
    def test_file_and_copy(filename, dst_dir, src_dir):
        dst = os.path.join(dst_dir, filename)
        if not Path(dst).is_file():
            src = os.path.join(src_dir, filename)
            shutil.copyfile(src, dst)
