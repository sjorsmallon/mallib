#ifndef INCLUDED_FILE_HANDLER
#define INCLUDED_FILE_HANDLER
#include <string>

class File_Handler
{
  char *m_buffer;
  std::string m_filename;

    public:
      // File_Handler() = delete;
      File_Handler(char* filename);
      File_Handler(const std::string& filename);
      File_Handler(const char* filename);
      ~File_Handler();

}