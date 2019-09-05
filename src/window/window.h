#ifndef INCLUDED_WINDOW_
#define INCLUDED_WINDOW_

class Window
{
	Platform_Window *window;
	//@Note: size_t?
	uint32_t m_width = 800;
	uint32_t m_height = 600;
	bool m_windowed = false; 

};


#endif