#ifndef INCLUDED_TEXTURE_MANAGER_
#define INCLUDED_TEXTURE_MANAGER_


struct Texture_Manager
{
	std::deque<uint32_t> free_texture_ids();
	static std::atomic<int> next_texture_id;
 

};

namespace tm
{
	
}




#endif
