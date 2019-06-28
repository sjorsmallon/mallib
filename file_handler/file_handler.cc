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
    // delete[] m_buffer;
}

void File_Handler::file_to_buffer()
{
    std::ifstream ifs(m_filename);
    std::string content;
    ifs.seekg(0, std::ios::end);   // move to the end of the file
    content.reserve(ifs.tellg()); 
    ifs.seekg(0, std::ios::beg); // move to the beginning.
    content.assign( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );

    //@Leak: this is a copy with new.
    m_buffer = new char[content.size() + 1];
    memcpy(m_buffer, content.c_str(), content.size() + 1);
}

