#ifndef INCLUDED_TIMED_FUNCTION_
#define INCLUDED_TIMED_FUNCTION_
#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <map>


//TODO(Sjors): parent id / scope depth.
// if the name already exists, we can just override ...

struct Timed_Function_Data
{
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> end;
	double duration;
};

struct Timed_Function
{
	// std::string name;
	// uint32_t id;
	// std::chrono::time_point<std::chrono::system_clock> start;
	// static uint32_t next_id;
	// static uint32_t scope_depth;
	static std::map<std::string, Timed_Function_Data> timed_functions;
	Timed_Function_Data& data;

	//@Note(Sjors): data is implicitly created through this constructor.
	Timed_Function(const std::string& name_in)
	:
		data(timed_functions[name_in])
	{
		data.start = std::chrono::system_clock::now();

		// next_id += 1;
		// if (scope_depth > 0)
		// {
		// 	// do something.
		// }
		// scope_depth += 1;
	}

	~Timed_Function()
	{
		data.end = std::chrono::system_clock::now();
		data.duration = static_cast<std::chrono::duration<double>>(data.end - data.start).count();
	}

	// delete the rest of the operators.
};

// inline uint32_t Timed_Function::next_id = 0; 
// inline uint32_t Timed_Function::scope_depth = 0; 
#define timed_function(function_name) auto timed_function = Timed_Function(function_name) 


#endif