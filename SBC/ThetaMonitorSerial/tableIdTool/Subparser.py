def add_subparsers(subparser):
    this_parser = subparser.add_parser(
        'prog', description='The Programmer tool. Writes data into the E2Prom'
                            ' of the device. It is used to set up a station with'
                            ' its necessary data, like the ID-Table or the Station-ID.'
                            ' One of the actions must be specified.')
    this_parser.add_argument(
        '--device', help="Serial-device to use, i.E. /dev/ttyUSB0. The"
                         " parameter in the settings-file will be overwritten.")
    group = this_parser.add_argument_group('action')
    group.add_argument(
        '-p', '--progIdTable', dest='tableIdFile', const='default',
        nargs='?',
        help="Write the contents of Id-Table file "
             "to the targets E2Prom.")
    group.add_argument(
        '-r', '--readIdTable', action='store_true', help="Read Id-Table from E2 and print it.")
    group.add_argument(
        '--setstationid', dest='stationId', type=int, help="arg=<uint32> decimal.  Write station-ID.")
    group.add_argument(
        '--getstationid', action='store_true', help="Read station-ID from device")


def overlay_settings(settings_in):
    settings = settings_in
    args = settings.args
    if args.device is not None:
        settings.set('common', 'serial_port', args.device)
