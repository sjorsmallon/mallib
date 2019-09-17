#include "file.ih"
#include <cstring>
#include <filesystem>

using std::filesystem fs;

// using std::vector<std::string> string_array; // what are conventions here?



bool file::file_to_buffer(const string& filename, char *buffer)
{
    std::ifstream ifs(filename);
    std::string content;
    ifs.seekg(0, std::ios::end);   // move to the end of the file
    content.reserve(ifs.tellg()); 
    ifs.seekg(0, std::ios::beg); // move to the beginning.
    content.assign( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

    //@Leak: this is a copy with new. is the size correct?
    buffer = new char[content.size() + 1];
    memcpy(buffer, content.c_str(), content.size() + 1); //could be strcpy?


}

std::vector<std::string> file::list_files_in_dir(const char* dir_name)
{
    fs::path active_dir = {dir_name};
    std::vector<std::string> file_names = {}; //@TODO: prealloc?

    if (fs::is_directory(active_dir)) //@TODO: assert
    {
        for (auto& file_name : fs::directory_iterator(dir_name))
        {
            file_names.emplace_back(file_name);
        }
    }

    return file_names;
}