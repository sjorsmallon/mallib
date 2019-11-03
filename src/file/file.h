#ifndef INCLUDE_FILE_
#define INCLUDE_FILE_
#include <string>
#include <filesystem>
#include <vector>
#include <string>

// struct Watch_List
// {
// 	// std::vector<std::string,std::chrono::duration<int, std::milli>> file_names;
// };


namespace file
{
  void file_to_string(const std::string& filename, std::string& target);
  std::vector<std::string> list_files_in_dir(const char* dir_name); 

  size_t get_file_size(const std::string& filename);

  void find_or_create_folder(const std::string& foldername);


  
  // void reload_file(const std::string& filename);
};

#endif

