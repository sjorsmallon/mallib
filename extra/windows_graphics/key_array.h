#ifndef INCLUDED_INPUT_
#define INCLUDED_IO_

namespace io
{
	struct Keyboard_State 
	{
		uint8_t data[256];

		operator[](size_t idx)
		{
			return data[idx];
		}
	}

	struct Mouse_State
	{
		uint8_t lmb;
		uint8_t rmb;
		float scroll_up;
		float scroll_down;
	}

};



#endif