#include "file_handler.ih"
#include <cstring>
#include <ifstream>
#include <string>

bool File_Handler::file_to_buffer(const string& filename, char *buffer)
{
    std::ifstream ifs(filename);
    std::string content;
    ifs.seekg(0, std::ios::end);   // move to the end of the file
    content.reserve(ifs.tellg()); 
    ifs.seekg(0, std::ios::beg); // move to the beginning.
    content.assign( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

    //@Leak: this is a copy with new.
    buffer = new char[s0.size() + 1];
    memcpy(s4, s0.c_str(), s0.size() + 1);;
}