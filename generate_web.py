#!/usr/bin/python -v
import os
from pathlib import Path
from colorama import init, Fore
from shutil import copyfile


print(Fore.GREEN + "Converting html to .h file")
src = Path("./src/dash.html")
dest = Path("./include/dash.h")

if not src.exists():
    print("No source file")
else:
    with open(dest, "w+") as f:
        f.write("#pragma once\n//Dont edit this file manualy\n")
        f.write('const char dash[] =R"rawliteral(')
        with open(src, "r") as f2:
            f.writelines(f2.readlines())
        f.write(')rawliteral";')
    print("File converted")
