#ifndef INCLUDED_FILE_HANDLER
#define INCLUDED_FILE_HANDLER
#include <string>

class File_Handler
{
  char *buffer;
  std::string filename;

    public:
      File_Handler();
      File_Handler(const std::string& filename);
      File_Handler(char* filename);
      File_Handler(const char* filename);



}