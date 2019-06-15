#include "file_handler.ih"
#include <cstring>
#include <ifstream>
#include <string>


//@Incomplete: when to return false?
bool file::file_to_buffer(const string& filename, char *buffer)
{
    bool success  = true;
    std::ifstream ifs(filename);
    std::string content;
    ifs.seekg(0, std::ios::end);   // move to the end of the file
    content.reserve(ifs.tellg()); 
    ifs.seekg(0, std::ios::beg); // move to the beginning.
    content.assign( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

    //@Leak: this is a copy with new, as there is no destructor here.
    //@Leak: this is not correct?
    buffer = new char[content.size() + 1];
    memcpy(buffer, content.c_str(), content.size() + 1);


}