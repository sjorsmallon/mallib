#ifndef INCLUDED_FILE_HANDLER
#define INCLUDED_FILE_HANDLER
#include <string>

class File_Handler
{
	FILE *file_ptr = nullptr;
	char *m_buffer;
	std::string m_filename;

    public:
		File_Handler(char* filename); 
		File_Handler(const std::string& filename);
		File_Handler(const char* filename);
		void file_to_buffer();
		~File_Handler();

		int file_size();
		FILE* file();

};

inline int File_Handler::file_size()
{
    int file_size = 0;
    fseek(file_ptr, 0, SEEK_END);
    file_size = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);

    return file_size;
}

//@Incomplete: we expose the raw file ptr, while we want to abstract. 
// but want to facilitate fread. do we wrap it?
inline FILE* File_Handler::file()
{
	return file_ptr;
}


#endif