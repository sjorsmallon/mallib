import os
import re

current_dir = os.path.dirname(os.path.abspath(__file__))
outputfile = open("output.cc", 'w')

for root, dirs, files in os.walk(current_dir):
    for name in files:
        if name.endswith((".h"):
            file = open('name', 'r').read()
            x = re.findall("struct [a-z,A-Z]* {[a-z,A-Z\ ';\r\n|\r|\n]*};")
            print(x)

            #consume the struct code block until closing brace.

            #append it to some file.
            # whatever
# for all files
