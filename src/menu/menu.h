#ifndef INCLUDED_MENU_
#define INCLUDED_MENU_
#include "../font/font.h"

namespace menu
{	
	enum class Mode
	{
		MODE_START,
		MODE_PAUSE
	};

	enum Menu_Item : int
	{
		START = 0,
		SETTINGS,
		EXIT,
		NONE
	};

	void init_menu();

	void draw_menu(/*font?*/);
	Menu_Item& active_start_menu_item();
	font::Font& menu_font();

};


#endif