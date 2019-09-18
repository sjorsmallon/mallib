#include "file.ih"
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem::v1;

// using std::vector<std::string> string_array; // what are conventions here?

void file::file_to_string(const string& filename, string& target)
{
    std::ifstream file(filename);
    file.seekg(0, std::ios::end);   
    target.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    target.assign((std::istreambuf_iterator<char>(t)),
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