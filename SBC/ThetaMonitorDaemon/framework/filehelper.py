import pathlib
import json


class Filehelper:
    @staticmethod
    def read_file_to_list(file: pathlib.Path):
        fileobject = open(file, 'r')
        content = []
        for line in fileobject:
            line = line.strip()
            content.append(line)
        fileobject.close()
        return content

    @staticmethod
    def dump_to_file(file: pathlib.Path, content: str):
        file = open(file, "w")
        file.write(content)
        file.close()

    @staticmethod
    def json_dump(file: pathlib.Path, json_structure):
        json_object = json.dumps(json_structure, indent=4)
        with open(file, "w") as outfile:
            outfile.write(json_object)

