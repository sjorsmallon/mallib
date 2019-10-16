#ifndef INCLUDED_FILE_WATCHER_
#define INCLUDED_FILE_WATCHER_
#include <experimental/filesystem>
#include <unordered_map>
#include <string>
#include <chrono>



struct File_Watcher
{
	std::vector<>
};



enum class Filestatus
{
	FILE_CREATED,
	FILE_MODIFIED,
	FILE_ERASED
}

class File_Watcher 
{
	std::unordered_map<std::string, std::filesystem::file_time_type> m_paths;
	std::string path_to_watch = {};
	std::chrono::duration<int, std::milli> delay = 0;

	public:
		FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay)
		:
			path_to_watch{path_to_watch},
			delay{delay}
		{
			for (auto &file : std::filesystem::recursive_directory_iterator(path_to_watch))
			{
				m_paths[file.path().string()] = std::filesystem::last_write_time(file);
			}
		}

		void start(const std::function<void (std::string, FileStatus)> &action)
		{
			auto it = m_paths.begin();
            while (it != m_paths.end())
            {
                 if (!std::filesystem::exists(it->first)) {
                     action(it->first, FileStatus::ERASED);
                     it = paths_.erase(it);
                 }
                 else
                     it++;                 
             }
 
             // Check if a file was created or modified
             for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch))
             {
                 auto current_file_last_write_time = std::filesystem::last_write_time(file);
                 if(paths_[file.path().string()] != current_file_last_write_time)
                 {
                     paths_[file.path().string()] = current_file_last_write_time;
                     action(file.path().string(), FileStatus::FILE_MODIFIED);
                 }
             }
         }
};

#endif // FILE_WATCHER