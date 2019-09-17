#ifndef INCLUDE_FILE_
#define INCLUDE_FILE_
#include <string>
#include <filesystem>
#include <vector>
#include <string>

namespace file
{
  bool file_to_buffer(const std::string& filename, char *buffer);
  // std::vector<std::string> list_files_in_dir(std::string& dir_name);
  std::vector<std::string> list_files_in_dir(const char* dir_name); 

}

#endif

