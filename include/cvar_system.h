
#ifndef INCLUDED_CVAR_SYSTEM_
#define INCLUDED_CVAR_SYSTEM_
#ifdef _WIN32 
	#pragma once
#endif

#include <cvar.h>
#include <map>
#include <string>

///--- Used instead of cvar_data_t, since _t suffixes are reserved.
using cvar_data_type = std::variant<bool, float, int32_t, std::string>;

namespace cvar
{

	///--- Used to create dictionaries of where which cvar is registered.
	enum class Cvar_Registry_Flags : int64_t
	{
		CVAR_SERIALIZE   =      0,  // 0
		CVAR_NOCHEAT     = 1 << 0,  // 1
		CVAR_CHEAT       = 1 << 1,  // 2
	    CVAR_SOUND       = 1 << 2,  // 4
	    CVAR_RENDERER    = 1 << 3   // 8
	};

	///--- The Cvar Itself.
	struct cvar_type
	{
		const char *name;
		cvar_data_type value;
		int64_t registry_flags;
		const char* description;
		cvar_data_type min_value;              //@Optional
		cvar_data_type max_value;              //@Optional
		std::vector<cvar_data_type> value_set; //@Optional
	}

	struct Cvar_System
	{
		std::map<const char*, cvar::Cvar> cvar_dict;
		// static int64_t cvar_ID = 0; // why did we do this?
	};

	//Singleton
	Cvar_System& cvar_system();



	void register_cvar_bool(
		const char* name, 
		cvar_data_type default_data, 
		int64_t cvar_registry_flags, 
		const char* description
	);

	void register_cvar_string(
		const char* name,
		cvar_data_type default_data,
		int64_t cvar_registry_flags,
		const char* description,
		std::vector<cvar_data_type> value_set
	);

	//@Note: these could be reduced to register_cvar_numeric, but
	// I just want it to be more clear what I'm trying to do.
	// If preferable, the value_set should not be used for
	// numeric types: categorical types should belong in enums etc.
	void register_cvar_float(
		const char* name,
		cvar_data_type default_data,
		int64_t cvar_registry_flags,
		const char* description,
		cvar_data_type min_value,
		cvar_data_type max_value,
		std::vector<cvar_data_type> value_set
	);

	void register_cvar_int(
		const char* name,
		cvar_data_type default_data,
		int64_t cvar_registry_flags,
		const char* description,
		cvar_data_type min_value,
		cvar_data_type max_value,
		std::vector<cvar_data_type> value_set 
	);

	bool getBool(const char* cvar_name);

	float getFloat(const char* cvar_name);

	int32_t getInt(const char* cvar_name);

	std::string getString(const char* cvar_name);


};


// #endif