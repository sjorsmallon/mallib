#include "file_handler.ih"

File_Handler::File_Handler(char* filename)
:
    m_filename(filename)
{
    file_to_buffer();
}

File_Handler::File_Handler(const std::string& filename)
:
    m_filename(filename)
{
    file_to_buffer();
}

File_Handler::File_Handler(const char* filename)
:
    m_filename(filename)
{
    file_to_buffer();
}

File_Handler::File_Handler(char* filename)
:
    m_filename(filename)
{
    file_to_buffer();
}

File_Handler::~File_Handler()
{
    delete[] m_buffer;
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
    memcpy(buffer, content.c_str(), content.size() + 1);
}

