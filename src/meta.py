import os
import re
import glob

print_flag = False

header_list = glob.glob("*/*.h")
for filename in header_list:   
    file = open(filename, 'r').readlines()
    for line in file:
        if re.search("struct ",line):        
            #todo: use the struct name for the main function.
            print_flag = True

        if re.search("};",line) and print_flag == True:
            print(line)
            print_flag = False
        if print_flag == True:
            print(line)

main = """
int main()
{
    // for all types, 
    std::is_trivial()
    std::is_basic_layout()
}
"""

