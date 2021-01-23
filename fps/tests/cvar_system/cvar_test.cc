#include "../../src/cvar_system.h"
#include <iostream>

static Cvar g_gravity("g_gravity", 		 int32_t{800},  CVAR_CHEAT, "gravity setting", int32_t{800}, int32_t{100});
static Cvar pm_movespeed("pm_movespeed", float{123.f},  CVAR_CHEAT, "player movement setting", float{0.234f}, float{123.f});

int main()
{

	auto& cvar_system = get_global_cvar_system();
	std::cerr << "global cvar system!\n" << '\n';
	
	set_int("g_gravity", 1);	
	// this should fail (and it does!).
	// set_float("g_gravity", 1.f);
	set_float("pm_movespeed", 12.f);


	set_int(g_gravity, 2);
	// this should fail
	// set_float(g_gravity, 2.f);
	set_float(pm_movespeed, 12.f);

	for (auto& [key,value]: cvar_system.cvars)
	{
		std::cerr << "cvar: " << key << '\n';
	} 
}