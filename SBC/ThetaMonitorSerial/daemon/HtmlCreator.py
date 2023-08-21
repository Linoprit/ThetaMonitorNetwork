import framework.settings
import dominate
from dominate.util import raw
from dominate.tags import *


class HtmlCreator:
    def __init__(self, settings_in: framework.settings.AppSettings):
        self.settings = settings_in

    def create_index_page(self):
        doc = dominate.document(title='Theta Monitor')


    def create_subpage(self, image_names):
        pass

    def create_head(self, doc: dominate.document):
        with doc.head:
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

    def create_testpage(self):
        doc = dominate.document(title='Theta Monitor')
        with doc.head:
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

        with doc:
            comment('Header')
            with div(id='header', cls='w3-container w3-theme w3-padding-8 w3-center'):
                h4('Theta Monitor (24h)')

            comment('Tab')
            with div(cls='w3-bar w3-orange'):
                button('Thetas', cls='w3-bar-item w3-button testbtn', onclick='openCity(event,\'Thetas\')')
                button('Luft', cls='w3-bar-item w3-button testbtn', onclick='openCity(event,\'Luft\')')
                button('Stats', cls='w3-bar-item w3-button testbtn', onclick='openCity(event,\'Statistik\')')

            comment('Menu')
            with div(cls='w3-bar w3-orange'):
                a('1 Woche', href='musterpage_oneWeek.html', cls='w3-bar-item w3-button w3-right')
                a('48h', href='musterpage_48h.html', cls='w3-bar-item w3-button w3-right')
                a('24h', href='musterpage.html', cls='w3-bar-item w3-button w3-right w3-dark-grey')

            comment('Theta-Tab')
            with div(id='Thetas', cls='ww3-container city w3-animate-opacity w3-center'):
                hr()
                img(src='plots/Temperatur_Werkstatt.png', cls='w3-border w3-padding center', alt='Werkstatt')
                br()
                img(src='plots/Temperatur_Lager.png', cls='w3-border w3-padding center', alt='Lager')
                br()
                img(src='plots/Temperatur_Innen.png', cls='w3-border w3-padding center', alt='Innen')
                hr()

            comment('Luft-Tab')
            with div(id='Luft', cls='ww3-container city w3-animate-opacity w3-center'):
                hr()
                img(src='plots/Luftdruck.png', cls='w3-border w3-padding center', alt='Luftdruck')
                br()
                img(src='plots/Luftfeuchte.png', cls='w3-border w3-padding center', alt='Luftfeuchte')
                hr()

            comment('Statistik-Tab')
            with div (id='Statistik', cls='w3-container city w3-animate-opacity w3-center'):
                hr()
                with table(cls='owntable w3-bordered center'):
                    with tr(cls='w3-theme'):
                        th('Sensor')
                        th('Temp')
                        th('Last Update')
                    with tr(cls='w3-white'):
                        td('ASN_O')
                        td('9.5')
                        td('2023-04-24 14:13:08')
                    with tr(cls='w3-white'):
                        td('GGE_U')
                        td('13.8125')
                        td('2023-04-24 14:13:08')
                    with tr(cls='w3-white'):
                        td('INNEN')
                        td('17.25')
                        td('2023-04-24 14:13:08')
                    with tr(cls='w3-white'):
                        td('LGR_O')
                        td('12.1875')
                        td('2023-04-24 14:13:08')
                    with tr(cls='w3-white'):
                        td('LGR_O')
                        td('12.1875')
                        td('2023-04-24 14:08:08')
                    with tr(cls='w3-white'):
                        td('GR_U')
                        td('11.5')
                        td('2023-04-24 14:13:08')
                    with tr(cls='w3-white'):
                        td('ST_O')
                        td('13.75')
                        td('2023-04-24 14:13:08')
                    with tr(cls='w3-white'):
                        td('WST_U')
                        td('12.1875')
                        td('2023-04-24 14:13:08')
                br()
                p('Plot alle Stationen \'lost packages\'')
                img(src='plots/Temperatur_Werkstatt.png', cls='w3-border w3-padding center', alt='Werkstatt')
                br()
                p('Plot alle Stationen \'valid sensors\'')
                img(src='plots/Temperatur_Lager.png', cls='w3-border w3-padding center', alt='Lager')
                hr()

            comment('Footer')
            with footer(cls='w3-container w3-theme-dark'):
                small('Theta Monitor Network (2023), Powered by')
                a('w3.css', href='https://www.w3schools.com/w3css/default.asp', target='_blank')

        doc.add(script(raw("""
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
        """)))

        #print(doc)
        filename = self.settings.add_output_path('index.html')
        with open(filename, 'w') as f:
            f.write(doc.render())
