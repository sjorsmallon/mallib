#include <thread>
#include "../src/task_system.h"

int main()
{

	Queue queue;
  	uint32_t thread_count = std::thread::hardware_concurrency(); 
  	tout << " hardware concurrency: " << thread_count << '\n';
  	Task_System task_system(thread_count);

  	for(size_t task_idx = 0; task_idx != 16; ++task_idx)
  	{
		add_task(task_system, [=](){
				for (size_t idx = 0; idx != 1000000; ++idx)
				{
					size_t ferfge = idx;
					size_t knogoo = idx + 15;
					size_t freeee = ferfge + knogoo;
				}
				tout << "thread " << std::this_thread::get_id() << " obtained task id:" << task_idx << "\n" << std::endl; 
			}
			);
  	}

  	std::string file;

  	add_task(task_system, [&](){

		std::ifstream t("file.txt");
		std::stringstream buffer;
		buffer << t.rdbuf();
		file = buffer.str();
  	});



	// add_task(task_system, [=](){
	// 	for (size_t idx = 0; idx != 1000000; ++idx)
	// 	{
	// 		size_t ferfge = idx;
	// 		size_t knogoo = idx + 15;
	// 		size_t freeee = ferfge + knogoo;
	// 	}
	// 		tout << "thread " << std::this_thread::get_id() << " obtained task id:" << "test" << "\n"; 
	// 	}
	// );



}