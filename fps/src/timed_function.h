#ifndef INCLUDED_TIMED_FUNCTION_
#define INCLUDED_TIMED_FUNCTION_
#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <map>
#include <cassert>
#include <numeric> // for accumulate
#include <array>

//@Macro(Sjors):
#define timed_function(function_name) auto timed_function = Timed_Function(function_name) 

//TODO(Sjors): parent id / scope depth.
// if the name already exists, we can just override ...

struct Time_Duration
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
	static std::map<std::string, Time_Duration> timed_functions;
	Time_Duration& data;

	static double get_duration(const std::string& function_name)
	{
		assert(Timed_Function::timed_functions.find(function_name) != Timed_Function::timed_functions.end());

		return Timed_Function::timed_functions[function_name].duration;
	}

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



// TIMER

constexpr const int buffer_size = 256;
struct Timer
{
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> end;
	double dt;
	std::array<double, buffer_size> buffer;
};

inline void timer_start(Timer& timer)
{
    timer.start = std::chrono::system_clock::now();

}
inline void timer_stop(Timer& timer)
{
	timer.end = std::chrono::system_clock::now();
    timer.dt = static_cast<std::chrono::duration<double>>(timer.end - timer.start).count();
}

inline void timer_maybe_log_average(Timer& timer, uint32_t frame_index)
{
    const auto buffer_idx = frame_index % buffer_size;
    timer.buffer[buffer_idx] = timer.dt;

    if (frame_index % buffer_size == 0) 
    {
        const double frame_average =  std::accumulate(timer.buffer.begin(), timer.buffer.end(), 0.0) / static_cast<double>(buffer_size);
        //@FIXME(Sjors): this is a degenerate way to persist this data every frame.
        Timed_Function::timed_functions["fps"].duration = 1.0 / frame_average;
        Timed_Function::timed_functions["average_frame_time"].duration = frame_average;
    }
}



#endif