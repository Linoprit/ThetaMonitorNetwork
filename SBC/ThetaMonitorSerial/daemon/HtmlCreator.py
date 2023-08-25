import dominate
import os
from datetime import datetime
from dominate.tags import *
from dominate.util import raw
from pathlib import Path
import framework.settings
import shutil
from daemon.HtmlConfigStructure import HtmlConfig


class HtmlCreator:
    def __init__(self,
                 settings_in: framework.settings.AppSettings,
                 html_conf: HtmlConfig):
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
            # print(doc)
            # filename = 'index.html'
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
        for one_div in divs:
            div_name = one_div["div_name"]
            with (div(id=div_name, cls='ww3-container city w3-animate-opacity w3-center')
                  as result):
                comment(div_name)
                hr()
                plots = one_div["plots"]
                for plot in plots:
                    filename = plot + "_" + content_config_name + ".png"
                    path = os.path.join("plots", filename)
                    img(src=path, cls=cls_str, alt=plot)
                    br()
                hr()
        return result

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

    # @staticmethod
    # def create_stats_tab_24h():
    #     result = div(id='Statistik', cls='w3-container city w3-animate-opacity w3-center')
    #     cls_str = 'w3-border w3-padding center'
    #     result.add(comment('Statistik-Tab'))
    #     result.add(hr())
    #     with table(cls='owntable w3-bordered center') as tbl:
    #         with tr(cls='w3-theme'):
    #             th('Sensor')
    #             th('Temp')
    #             th('Last Update')
    #         with tr(cls='w3-white'):
    #             td('ASN_O')
    #             td('9.5')
    #             td('2023-04-24 14:13:08')
    #         with tr(cls='w3-white'):
    #             td('GGE_U')
    #             td('13.8125')
    #             td('2023-04-24 14:13:08')
    #         with tr(cls='w3-white'):
    #             td('INNEN')
    #             td('17.25')
    #             td('2023-04-24 14:13:08')
    #         with tr(cls='w3-white'):
    #             td('LGR_O')
    #             td('12.1875')
    #             td('2023-04-24 14:13:08')
    #         with tr(cls='w3-white'):
    #             td('LGR_O')
    #             td('12.1875')
    #             td('2023-04-24 14:08:08')
    #         with tr(cls='w3-white'):
    #             td('GR_U')
    #             td('11.5')
    #             td('2023-04-24 14:13:08')
    #         with tr(cls='w3-white'):
    #             td('ST_O')
    #             td('13.75')
    #             td('2023-04-24 14:13:08')
    #         with tr(cls='w3-white'):
    #             td('WST_U')
    #             td('12.1875')
    #             td('2023-04-24 14:13:08')
    #     result.add(tbl)
    #     result.add(br())
    #     result.add(p('Plot alle Stationen \'lost packages\''))
    #     result.add(img(src='plots/Temperatur_Werkstatt.png', cls=cls_str, alt='Werkstatt'))
    #     return result

    # @staticmethod
    # def create_theta_tab_24h():
    #     cls_str = 'w3-border w3-padding center'
    #     with (div(id='Thetas', cls='ww3-container city w3-animate-opacity w3-center')
    #           as result):
    #         comment('Theta-Tab')
    #         hr()
    #         img(src='plots/Temperatur_Werkstatt.png', cls=cls_str, alt='Werkstatt')
    #         br()
    #         img(src='plots/Temperatur_Lager.png', cls=cls_str, alt='Lager')
    #         br()
    #         img(src='plots/Temperatur_Innen.png', cls=cls_str, alt='Innen')
    #         hr()
    #     return result

    # @staticmethod
    # def create_luft_tab_24h():
    #     cls_str = 'w3-border w3-padding center'
    #     with (div(id='Luft', cls='ww3-container city w3-animate-opacity w3-center')
    #           as result):
    #         comment('Luft-Tab')
    #         hr()
    #         img(src='plots/Luftdruck.png', cls=cls_str, alt='Luftdruck')
    #         br()
    #         img(src='plots/Luftfeuchte.png', cls=cls_str, alt='Luftfeuchte')
    #         hr()
    #     return result
