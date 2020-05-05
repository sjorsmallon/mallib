
#include <cvar_system.h>
// globals

cvar::Cvar_System& cvar::cvar_system()
{
	static cvar::Cvar_System cvar_system;
	return cvar_system;
}


void register_cvar(
	const char* name, 
	cvar_data_type data, 
	int64_t cvar_flags, 
	const char* description,
	cvar_data_type min_value,
	cvar_data_type max_value,
	std::vector<cvar_data_type> value_set;
	)
{	
	auto &cvar_system = cvar::cvar_system();
	cvar::Cvar new_cvar{};
	new_cvar.name = name;
	new_cvar.data = data;
	new_cvar.preview_text = preview_text;
	new_cvar.flags = flags;
	cvar_system[name] = cvar;

	//@TODO: assert that min_value && max_value are the same type as data.
	//@TODO: can we exclude them for bool? as well as value_set?

	// MOVE_CVARS_TO_DICT

	
}

	