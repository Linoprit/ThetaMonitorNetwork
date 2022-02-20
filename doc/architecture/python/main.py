# This is a sample Python script.
import os
from hpp2plantuml import CreatePlantUMLFile, Diagram
# you must have installed Jinja2, version 2.10
# pip3 uninstall jinja2
# pip3 install Jinja2==2.10.1
import os
import subprocess


def create_file_list(start_path, suffix):  # suffix without dot, i.e. 'txt'
    list_of_files = {}
    for (dirpath, dirnames, filenames) in os.walk(start_path):
        for filename in filenames:
            if filename.endswith('.' + suffix):
                list_of_files[filename.split('.')[0]] = os.sep.join([dirpath, filename])
    return list_of_files


def call_plantuml(puml_file, out_path):
    print("Processing contained puml in " + puml_file, end=' => ')
    # plantuml prints a warning to stderr, if no puml-code was found in the file
    result = subprocess.Popen(['plantuml', puml_file, " -o ", out_path],
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    out, err = result.communicate()
    err = err.decode('utf-8')

    if err.startswith("Warning: no image in"):
        print("No puml-code found.")
    elif err.startswith("Error line 0 in file:"):
        print("File is empty.")
    elif not err and not out:
        print("OK.")
    else:
        print(out)
        print(err)


if __name__ == '__main__':
    sourcePath = "../../../Framework"
    pumlPath = "./plantUml"
    pngPath = "../png"

    # collect all header-files
    file_list = create_file_list(sourcePath, 'h')

    # if puml-code is in the files, process it
    for file, sourcePath in file_list.items():
        call_plantuml(sourcePath, os.path.join(os.getcwd(), pngPath))

    # hpp2plantuml
    for file, sourcePath in file_list.items():
        outPath = os.path.join(pumlPath, file + '.puml')
        print ("extracting " + sourcePath)
        CreatePlantUMLFile(sourcePath, outPath,
                           template_file=None,
                           flag_dep=True)

    # convert the new pumls to png
    file_list = create_file_list(pumlPath, 'puml')
    for file, sourcePath in file_list.items():
        print("converting ", sourcePath)
        call_plantuml(sourcePath, os.path.join(os.getcwd(), pngPath))

    # old code, delete it later.
    # # create pngs with planuUml
    # file_list = create_file_list(pumlPath, 'puml')
    # for file, sourcePath in file_list.items():
    #     # outPath = os.path.join(pngPath, file + '.puml')
    #     print("converting ", sourcePath)
    #     # processes_file(self, filename, outfile=None, errorfile=None, directory=''):
    #     plantUml = PlantUML('http://www.plantuml.com/plantuml/img/')
    #     try:
    #         plantUml.processes_file(filename=sourcePath)
    #     except AttributeError:
    #         print("\t file is empty")

    # # move the pngs to target-dir
    # #wildcard = pumlPath + "/*.png"
    # file_list = create_file_list(pumlPath, 'png')
    # for target, source in file_list.items():
    #     print(target, source)
    #     shutil.move(source, os.path.join(pngPath, target + ".png"))


# def del_puml(start_path):
#     list_of_pumls = {}
#     for (dirpath, dirnames, filenames) in os.walk(start_path):
#         for filename in filenames:
#             if filename.endswith('.puml'):
#                 list_of_pumls[filename] = os.sep.join([dirpath, filename])
#
#     for file, path in list_of_pumls.items():
#         print(file + " " + path)
#         os.remove(path)
#     return list_of_pumls
