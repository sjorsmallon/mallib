#ifndef INCLUDED_FILE_
#define INCLUDED_FILE_
#include <string>
#include <vector>
#include "logr.h"
#include <filesystem>
#include <fstream>

inline void file_to_string(const std::string& filename, std::string& target)
{
    std::ifstream file{filename};
    
    if (!file.is_open())
    {
        logr::report_warning("[file] filename: {} does not exist. returning empty string.\n" , filename);
        target = {};
        return;
    }

    std::filesystem::path path{filename};
    auto file_size = std::filesystem::file_size(path);

    // logr::report("[file] the size of {} is {} bytes.\n", filename, file_size);
    target.reserve(file_size);
    target.assign((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
}

inline std::vector<std::string> list_files_in_dir(const std::string& dir_name)
{
    std::filesystem::path active_dir{dir_name};
    std::vector<std::string> file_names{}; 

    assert(std::filesystem::is_directory(active_dir));

    for (auto& file_name : std::filesystem::directory_iterator(dir_name))
    {
        std::string file_name_string = file_name.path().filename().string();
        file_names.push_back(file_name_string);
    }

    return std::move(file_names);
}

// correct but C file size.
// long GetFileSize(std::string filename)
// {
//     struct stat stat_buf;
//     int rc = stat(filename.c_str(), &stat_buf);
//     return rc == 0 ? stat_buf.st_size : -1;
// }

// or 

// long FdGetFileSize(int fd)
// {
//     struct stat stat_buf;
//     int rc = fstat(fd, &stat_buf);
//     return rc == 0 ? stat_buf.st_size : -1;
// }


#endif