import pathlib


class Filehelper:
    @staticmethod
    def read_file_to_list(file: pathlib.Path):
        fileobj = open(file, 'r')
        content = []
        for line in fileobj:
            line = line.strip()
            if line.startswith('#') or line == '\n' or not line :
                continue
            content.append(line)
        fileobj.close()
        return content

    @staticmethod
    def dump_to_file(file: pathlib.Path, content: str):
        file = open(file, "w")
        file.write(content)
        file.close()
