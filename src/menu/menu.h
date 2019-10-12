#ifndef INCLUDED_MENU_
#define INCLUDED_MENU_

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

	void draw_menu();

	Menu_Item& active_start_menu_item();

};


#endif