#ifndef INCLUDED_GLOBALS_
#define INCLUDED_GLOBALS_

namespace globals
{
	// asset folders
	const static inline std::string font_folder = "../fonts/";
	const static inline std::string music_folder = "../music/";
	const static inline std::string texture_folder = " ../textures/";


	struct Window_Settings
	{
		Vec2i window_dimensions;
	};

	struct Global_Input
	{
		Vec2i mouse_coords;
		std::vector<Key_Input> input_queue;
	};



	// mouse position
	// window dimensions
	// render dimensions
	// game state? (i.e. paused, whatever.)
	// what music is playing?

	enum class Active_Menu_Item
	{
		START,
		SETTINGS,
		EXIT
	};

	enum class Game_State
	{
		Active,
		Paused
	}

	//@serialize.
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
	


};

#endif






