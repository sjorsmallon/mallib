import os
import re
import glob

print_flag = False


includes = """
#include <type_traits>
#include <iostream>
"""
print(includes)


struct_collection = [];
header_list = glob.glob("*/*.h")
for filename in header_list:
    if "stb" not in filename:
        file = open(filename, 'r').readlines()
        for line in file:
            union = re.search("union ", line)
            struct = re.search("struct ",line)
            comment = re.search("//",line)
            # if commented out, we out.
            if struct == None:
                if print_flag == False:
                    continue

            if struct != None:
                struct_start = struct.span()[0]
                struct_end  = struct.span()[1]
                if comment != None:
                    comment_start = comment.span()[0]
                    if comment_start < struct_start:
                        continue
                else:
                    print("//" + filename)
                    words = line.split()
                    for index, word in enumerate(words):
                        if word == "struct":
                            # sigh. in the union, we have anonymous structs.
                            my_index = index if index == len(words) -1 else index + 1
                            struct_collection.append(words[my_index])

            #todo: use the struct name for the main function.
            print_flag = True
            if re.search("};",line) and print_flag == True:
                print(line)
                print_flag = False
            if print_flag == True:
                print(line)

main_start = """
int main()
{
    std::cout << std::boolalpha
    // for all types, 
"""
print (main_start)

for struct in struct_collection:
    # print("   " + struct + " " + struct.lower() + ";")
    print
    print("std::cout << std::boolalpha << " +"\"" + struct + " is trivial: \"" +  "std::is_trivial<"+struct+">::value" + "<< std::endl;")
    print("std::cout << std::boolalpha << " +"\"" + struct + " is standard layout: \"" + "std::is_standard_layout<"+struct+">::value" + "<< std::endl;") 
    print(" ")



main_end = """ 

}
"""
print(main_end)
print(struct_collection)
