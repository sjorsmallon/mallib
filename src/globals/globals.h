#ifndef INCLUDED_GLOBALS_
#define INCLUDED_GLOBALS_

namespace globals
{
	// asset folders
	const static inline std::string font_folder = "../fonts/";
	const static inline std::string sound_folder = "../sounds/";
	const static inline std::string texture_folder = " ../textures/";


	enum class Active_Menu_Item
	{
		START,
		SETTINGS,
		EXIT
	};


	struct Campaign_State
	{
		uint32_t current_level;
		uint32_t master_volume;
		uint32_t music_volume;
	};

	// in pixels?
	uint32_t large_font_size;
	uint32_t huge_font_size;
	uint32_t small_font_size;
	
	// mouse position
	// window dimensions
	// render dimensions
	// game state? (i.e. paused, whatever.)
	// what music is playing?

};

#endif






