#ifndef INCLUDED_KEY_ARRAY_
#define INCLUDED_KEY_ARRAY_


class Key_Array
{
	uint8_t *data;
	size_t size = 256;
	public:
		Key_Array()
		//:
		{
			data = new uint8[256];
		}
		~Key_Array()
		{
			delete[] data;
		}
};




#endif