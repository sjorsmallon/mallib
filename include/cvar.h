#ifndef INCLUDED_CVAR_
#define INCLUDED_CVAR_
#include <map>

enum class Cvar_Flags : int64_t
{
	CVAR_SERIALIZE =       0,  // 0
	CVAR_NOCHEAT     = 1 << 0,  // 1
	CVAR_CHEAT       = 1 << 1,  // 2
    CVAR_SOUND       = 1 << 2,  // 4
    CVAR_RENDERER    = 1 << 3   // 8
};


using cvar_data_type = std::variant<bool, float, int32_t>;

namespace cvar
{
	struct Cvar
	{
		const char *name;
		cvar_data_type value;
		int64_t Cvar_Flags;
		const char* preview_text;
	}

	
}


#endif


