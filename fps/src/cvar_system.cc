#include "cvar_system.h"
#include <utility>

Cvar_System& get_global_cvar_system()
{
	static Cvar_System cvar_system;
	return cvar_system;
}

void register_cvar(Cvar_System& cvar_system, Cvar* cvar)
{
	cvar_system.cvars[std::string(cvar->name)] = cvar;
}


// bool
Cvar::Cvar(
		const char* name_in,
		bool default_data_in,
		int64_t cvar_registry_flags_in,
		const char* description_in)
	:
		name(name_in),
		data(default_data_in), 
		registry_flags(cvar_registry_flags_in),
		description(description_in),
		value_set()
{

	register_cvar(get_global_cvar_system(), this);
}

// string
Cvar::Cvar(
		const char* name_in,
		std::string default_data_in,
		int64_t cvar_registry_flags_in,
		const char* description_in,
		std::vector<std::string> value_set_in)
	:
		name(name_in),
		data(default_data_in), 
		registry_flags(cvar_registry_flags_in),
		description(description_in),
		value_set(value_set_in.begin(), value_set_in.end())
{
	register_cvar(get_global_cvar_system(), this);
}

// float
Cvar::Cvar(
		const char* name_in,
		float default_data_in,
		int64_t cvar_registry_flags_in,
		const char* description_in,
		float min_value_in,
		float max_value_in,
		std::vector<float> value_set_in)	
	:

		name(name_in),
		data(default_data_in), 
		registry_flags(cvar_registry_flags_in),
		description(description_in),
		min(min_value_in),
		max(max_value_in),
		value_set(value_set_in.begin(), value_set_in.end())
{
	register_cvar(get_global_cvar_system(), this);
}


// int32_t
Cvar::Cvar(
		const char* name_in,
		int32_t default_data_in,
		int64_t cvar_registry_flags_in,
		const char* description_in,
		int32_t min_value_in,
		int32_t max_value_in,
		std::vector<int32_t> value_set_in)
	:

		name(name_in),
		data(default_data_in), 
		registry_flags(cvar_registry_flags_in),
		description(description_in),
		min(min_value_in),
		max(max_value_in),
		value_set(value_set_in.begin(), value_set_in.end())
{
	register_cvar(get_global_cvar_system(), this);
}

float get_float(const Cvar& cvar)
{
	return std::get<float>(cvar.data);
}
std::string get_string(const Cvar& cvar)
{
	return std::get<std::string>(cvar.data);
}

bool get_bool(const Cvar& cvar)
{
	return std::get<bool>(cvar.data);
}

int32_t get_int(const Cvar& cvar)
{
	return std::get<int32_t>(cvar.data);
}

void set_float(Cvar& cvar, float value)
{
	assert(std::holds_alternative<float>(cvar.data));
	cvar.data = value;
}

void set_string(Cvar& cvar, std::string value)
{
	assert(std::holds_alternative<std::string>(cvar.data));
	cvar.data = value;
}
void set_bool(Cvar& cvar, bool value)
{
	assert(std::holds_alternative<bool>(cvar.data));
	cvar.data = value;
}
void set_int(Cvar& cvar, int32_t value)
{
	assert(std::holds_alternative<int32_t>(cvar.data));
	cvar.data = value;
}

bool get_bool(const char* cvar_name)
{
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());

	return std::get<bool>(cvar_system.cvars[cvar_name]->data);
}

float get_float(const char* cvar_name)
{
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());

	return std::get<float>(cvar_system.cvars[cvar_name]->data);
}

int32_t get_int(const char* cvar_name)
{
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());

	return std::get<int32_t>(cvar_system.cvars[cvar_name]->data);
}

std::string get_string(const char* cvar_name)
{
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());


	return std::get<std::string>(cvar_system.cvars[cvar_name]->data);
}

void set_float(const char* cvar_name, float value)
{	
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());

	auto& cvar = cvar_system.cvars[cvar_name];
	assert(std::holds_alternative<float>(cvar->data));

	cvar->data = value;
}

void set_bool(const char* cvar_name, bool value)
{
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());

	auto& cvar = cvar_system.cvars[cvar_name];
	assert(std::holds_alternative<bool>(cvar->data));

	cvar_system.cvars[cvar_name]->data = value;
}

void set_int(const char* cvar_name, int32_t value)
{
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());

	auto& cvar = cvar_system.cvars[cvar_name];
	assert(std::holds_alternative<int32_t>(cvar->data));

	cvar->data = value;
}

void set_string(const char* cvar_name, std::string value)
{
	auto& cvar_system = get_global_cvar_system();
	assert(cvar_system.cvars.find(cvar_name) != cvar_system.cvars.end());

	auto& cvar = cvar_system.cvars[cvar_name];
	assert(std::holds_alternative<std::string>(cvar->data));

	cvar->data = value;
}

