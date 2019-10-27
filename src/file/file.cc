#include "file.ih"
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include <experimental/filesystem>

#include "fmt/core.h"

#include <sys/stat.h>

using namespace std;
namespace fs = std::experimental::filesystem::v1;


void file::file_to_string(const string& filename, string& target)
{
    //@FIXME: seekg & tellg cannot be properly used to determine file size. 
    // see some discussions about that. Prefer the get_file_size function defined in this
    // namespace to find the file size.
    std::ifstream file(filename);
    if (!file.is_open())
    {
        fmt::print("[file] filename: {} does not exist. returning empty string.\n", filename);
        target = {};
        return;
    }

    file.seekg(0, std::ios::end);   
    target.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    target.assign((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
}

std::vector<std::string> file::list_files_in_dir(const char* dir_name)
{
    // std::string dir_name(Dir_name);
    fs::path active_dir{dir_name};
    std::vector<std::string> file_names = {}; //@TODO: prealloc?

    if (fs::is_directory(active_dir)) //@TODO: assert
    {
        for (auto& file_name : fs::directory_iterator(dir_name))
        {
            file_names.push_back(file_name.path().filename().string());
        }
    }

    return file_names;
}

// returns 0 when something's not right.
size_t file::get_file_size(const std::string& filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : 0;
}

// platform specific.
void find_or_create_folder(const std::string& foldername)
{
    fs::create_directory(foldername);
    // otherwise:
    // ASSUMED INCLUDES
// #include <string> // required for std::string
// #include <sys/types.h> // required for stat.h
// #include <sys/stat.h> // no clue why required -- man pages say so

// std::string sPath = "/tmp/test";
// mode_t nMode = 0733; // UNIX style permissions
// int nError = 0;
// #if defined(_WIN32)
//   nError = _mkdir(sPath.c_str()); // can be used on Windows
// #else 
//   nError = mkdir(sPath.c_str(),nMode); // can be used on non-Windows
// #endif
// if (nError != 0) {
//   // handle your error here
// }

}
