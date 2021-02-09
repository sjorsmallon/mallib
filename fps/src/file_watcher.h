#ifndef INCLUDED_FILE_WATCHER_
#define INCLUDED_FILE_WATCHER_

#include <filesystem>
#include <chrono>
#include <unordered_map>
#include <string>
#include <functional>

#include "logr.h"

// Define available file changes
enum class File_Status {created, modified, erased};


struct File_Watcher
{
	std::vector<std::string> folders;
    std::map<std::string, std::filesystem::file_time_type> paths = std::map<std::string, std::filesystem::file_time_type>();
    std::map<std::string, std::function<void(const std::string&)>> actions;
    std::chrono::duration<float, std::milli> interval;
    std::chrono::time_point<std::chrono::system_clock> last_time_checked;

    File_Watcher() = default;
    File_Watcher(const File_Watcher& file_watcher) = delete;
    File_Watcher(File_Watcher&& file_watcher) = delete;
    ~File_Watcher() = default;
};

inline void watch_folder(
	File_Watcher& file_watcher,
	const char* folder_path,
	std::chrono::duration<float, std::milli> interval_in,
	const std::function<void(const std::string& filename)> &action)
{
	file_watcher.interval = interval_in;
	file_watcher.last_time_checked = std::chrono::system_clock::now();

    assert(std::filesystem::is_directory(folder_path));
    for (auto&& file: std::filesystem::recursive_directory_iterator(folder_path))
    {
        file_watcher.paths[file.path().string()] = std::filesystem::last_write_time(file);
        file_watcher.actions[file.path().string()] = action;
        file_watcher.folders.push_back(folder_path);
    }
}


inline void check_files_for_change(File_Watcher& file_watcher)
{
    // checks for deleted files., but don't do anything with that information.
    {
        auto it = file_watcher.paths.begin();
        while (it != file_watcher.paths.end())
        {
            if (!std::filesystem::exists(it->first))
            {
                // action(it->first, File_Status::erased);
                it = file_watcher.paths.erase(it);
            }
            else
            {
                ++it;
            }                    
        }
    }

    for (auto& folder: file_watcher.folders)
    {
    	 // Check if a file was modified, or created. Ignore created files.
	    for(auto &file : std::filesystem::recursive_directory_iterator(folder))
	    {
	        auto current_file_last_write_time = std::filesystem::last_write_time(file);

	        // File creation
	        // if(!contains(file.path().string()))
	        // {
	        //     file_watcher.paths[file.path().string()] = current_file_last_write_time;
	        //     action(file.path().string(), File_Status::created);
	        // // File modification
	        // }
	        // else 
	        {
	            if(file_watcher.paths[file.path().string()] != current_file_last_write_time)
	            {
                    logr::report_error("[file_watcher] detected file change!\n");
                    // while(true){}
	                file_watcher.paths[file.path().string()] = current_file_last_write_time;
	                file_watcher.actions[file.path().string()](folder);
	            }
	        }
	    }
    }
}

inline void maybe_watch_files(File_Watcher& file_watcher, std::chrono::time_point<std::chrono::system_clock> timestamp)
{
	auto start = file_watcher.last_time_checked;
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - start).count() > file_watcher.interval.count())
	{

		check_files_for_change(file_watcher);
		file_watcher.last_time_checked = timestamp;
	}
}




#endif