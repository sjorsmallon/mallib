
#include <cvar_system.h>
#include <fmt/core.h>


// globals
cvar::Cvar_System& cvar::cvar_system()
{
	static cvar::Cvar_System cvar_system;
	return cvar_system;
}


void cvar::register_cvar_bool(
	const char* name, 
	cvar_data_type default_data, 
	int64_t cvar_registry_flags, 
	const char* description
	)
{	
	auto& cvar_system = cvar::cvar_system();
	cvar::cvar_type new_cvar{};
	new_cvar.name = name;
	new_cvar.value = default_data;
	new_cvar.description = description;
	new_cvar.description = description;
	new_cvar.registry_flags = cvar_registry_flags;

	if (cvar_system.cvar_dict.find(name) != cvar_system.cvar_dict.end())
	{
		fmt::print("[cvar] [WARNING] overriding cvar: [{}].\n", new_cvar.name);
	}
	else
	{
		fmt::print("[cvar] Registered cvar: [{}].\n", new_cvar.name);
	}

	//@Memory: implicit creation.
	cvar_system.cvar_dict[name] = new_cvar;

}

void cvar::register_cvar_float(
	const char* name,
	cvar_data_type default_data,
	int64_t cvar_registry_flags,
	const char* description,
	cvar_data_type min_value,
	cvar_data_type max_value,
	std::vector<cvar_data_type> value_set
)
{
	auto& cvar_system = cvar::cvar_system();
	cvar::cvar_type new_cvar{};

	new_cvar.name = name;
	new_cvar.value =  default_data;
	new_cvar.registry_flags = cvar_registry_flags;
	new_cvar.description = description;
	new_cvar.min_value = min_value;
	new_cvar.max_value = max_value;
	new_cvar.value_set = value_set;

	if (cvar_system.cvar_dict.find(name) != cvar_system.cvar_dict.end())
	{
		fmt::print("[cvar] [WARNING] overriding cvar: [{}]. \n", new_cvar.name);
	}
	else
	{
		fmt::print("[cvar] Registered cvar: [{}]. \n", new_cvar.name);
	}
	//@Memory: implicit creation.
	cvar_system.cvar_dict[name] = new_cvar;
};


void cvar::register_cvar_string(
	const char* name,
	cvar_data_type default_data,
	int64_t cvar_registry_flags,
	const char* description,
	std::vector<cvar_data_type> value_set
)
{
	auto& cvar_system = cvar::cvar_system();
	cvar::cvar_type new_cvar{};

	new_cvar.name = name;
	new_cvar.value =  default_data;
	new_cvar.registry_flags = cvar_registry_flags;
	new_cvar.description = description;
	new_cvar.value_set = value_set;

	if (cvar_system.cvar_dict.find(name) != cvar_system.cvar_dict.end())
	{
		fmt::print("[cvar] [WARNING] overriding cvar: [{}]. \n", new_cvar.name);
	}
	else
	{
		fmt::print("[cvar] Registered cvar: [{}]. \n", new_cvar.name);
	}
	//@Memory: implicit creation.
	cvar_system.cvar_dict[name] = new_cvar;

}


void cvar::register_cvar_int(
	const char* name,
	cvar_data_type default_data,
	int64_t cvar_registry_flags,
	const char* description,
	cvar_data_type min_value,
	cvar_data_type max_value,
	std::vector<cvar_data_type> value_set
)
{
	auto& cvar_system = cvar::cvar_system();
	cvar::cvar_type new_cvar{};

	new_cvar.name = name;
	new_cvar.value =  default_data;
	new_cvar.registry_flags = cvar_registry_flags;
	new_cvar.description = description;
	new_cvar.min_value = min_value;
	new_cvar.max_value = max_value;
	new_cvar.value_set = value_set;


	if (cvar_system.cvar_dict.find(name) != cvar_system.cvar_dict.end())
	{
		fmt::print("[cvar] [WARNING] overriding cvar: [{}].\n", new_cvar.name);
	}	else
	{
		fmt::print("[cvar] Registered cvar: [{}]. \n", new_cvar.name);
	}

	//@Memory: implicit creation.
	cvar_system.cvar_dict[name] = new_cvar;
}

bool cvar::get_bool(const char* target_cvar_name)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<bool>(found_cvar.value))
	    {
	    	return std::get<bool>(found_cvar.value);
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] found cvar holds incorrect type. returning default value.\n");
	    	return false;
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] cvar bool not found: [{}]. returning default value.\n", target_cvar_name);
		return false;
	}
}

float cvar::get_float(const char* target_cvar_name)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<float>(found_cvar.value))
	    {
	    	return std::get<float>(found_cvar.value);
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] found cvar holds incorrect type. returning default value.\n");
	    	return float{0.f};
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] cvar float not found: [{}]. returning default value.\n", target_cvar_name);
		return float{0.f};
	}
}

int32_t cvar::get_int(const char* target_cvar_name)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<int32_t>(found_cvar.value))
	    {
	    	return std::get<int32_t>(found_cvar.value);
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] found cvar holds incorrect type. returning default value.\n");
	    	return int32_t{0};
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] cvar float not found: [{}]. returning default value.\n", target_cvar_name);
		return int32_t{0};
	}
}


std::string cvar::get_string(const char* target_cvar_name)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<std::string>(found_cvar.value))
	    {
	    	return std::get<std::string>(found_cvar.value);
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] found cvar holds incorrect type. returning empty string.\n");
	    	return std::string{};
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] cvar float not found: [{}]. returning empty string.\n", target_cvar_name);
		return std::string{};
	}
}


void cvar::set_float(const char* target_cvar_name, float value)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<float>(found_cvar.value))
	    {
	    	found_cvar.value = value;
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] set_float: cvar holds incorrect type. not updating anything.\n");
	    	return;
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] set_float: cvar not found: [{}]. not updating anything.\n", target_cvar_name);
		return;
	}
}

void cvar::set_int(const char* target_cvar_name, int32_t value)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<int32_t>(found_cvar.value))
	    {
	    	found_cvar.value = value;
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] set_int: cvar holds incorrect type. not updating anything.\n");
	    	return;
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] set_int: cvar not found: [{}]. returning empty string.\n", target_cvar_name);
		return;
	}
}

void cvar::set_bool(const char* target_cvar_name, bool value)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<bool>(found_cvar.value))
	    {
	    	found_cvar.value = value;
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] set_bool: cvar holds incorrect type. not updating.\n");
	    	return;
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] set_bool: could not find cvar: [{}]. not updating. \n", target_cvar_name);
		return;
	}
}

void cvar::set_string(const char* target_cvar_name, std::string value)
{
	auto& cvar_system = cvar::cvar_system();

	if (cvar_system.cvar_dict.find(target_cvar_name) != cvar_system.cvar_dict.end())
	{
		cvar_type& found_cvar = cvar_system.cvar_dict[target_cvar_name];

	    if (std::holds_alternative<std::string>(found_cvar.value))
	    {
	    	found_cvar.value = value;
	    }
	    else
	    {
	    	fmt::print("[cvar] [WARNING] set_bool: cvar holds incorrect type. not updating.\n");
	    	return;
	    }
	}
	else // not found.
	{
		fmt::print("[cvar] [WARNING] set_bool: could not find cvar: [{}]. not updating\n", target_cvar_name);
		return;
	}
}