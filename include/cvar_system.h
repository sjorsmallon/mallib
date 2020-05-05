
#ifndef INCLUDED_CVAR_SYSTEM_
#define INCLUDED_CVAR_SYSTEM_
#ifdef _WIN32 
	#pragma once
#endif

#include <cvar.h>
#include <map>
#include <string>


namespace cvar
{
	struct Cvar_System
	{
		std::map<std::string, cvar::Cvar> all_cvars;
		static int64_t cvar_ID = 0;
	};

	Cvar_System& cvar_system();



};


// #endif