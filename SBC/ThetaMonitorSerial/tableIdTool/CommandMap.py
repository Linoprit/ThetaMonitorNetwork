class CommandMap:
    '''
    We use this class to keep things simple, if we change or augment the
    command-syntax on the Micro.
    The command-list, is taken from Interpreter.cpp. Format of the dict is
    [command-alias: Stm-command-string].
    '''

    def __init__(self):
        self.commands = {
            "setSensIdCmd": "setSensId",
            "getSensIdTableCmd": "getSensIdTable",
            "setStationIdCmd": "setStationId",
            "getStationIdCmd": "getStationId",
            "clrSensIdTableCmd": "clrSensIdTable",
            "shutupCmd": "shutup",
            "talkCmd": "talk",
            "calcHashCmd": "calcHash",
            "rebootCmd": "reboot",
            "exitCmd": "exit",
        }

    def getCmdString(self, alias_str: str):
        return self.commands[alias_str]
