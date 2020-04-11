#include "file_handler.ih"
#include <cstring>

File_Handler::File_Handler(char* filename)
// :
//     m_filename(filename)
{
    file_to_buffer();
}

File_Handler::File_Handler(const std::string& filename)
// :
//     m_filename(filename)
{
    file_to_buffer();
}

File_Handler::File_Handler(const char* filename)
// :
//     m_filename(filename)
{
    file_ptr = fopen(filename, "r");
    if (file_ptr == nullptr)
    {
        printf("unable to open file: %s\n", m_filename.c_str());
    }
    else
    {
        printf("opened file\n");
    }
}


File_Handler::~File_Handler()
{
    fclose(file_ptr);
    //@Leak: delete[] m_buffer;
}

