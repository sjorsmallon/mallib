#ifndef INCLUDED_MEMORY_POOL
#define INCLUDED_MEMORY_POOL
#include <vector>

using byte = unsigned char;

namespace mem
{

	// bool is_dirty

	struct Pool
	{
	  byte* data; 
	  size_t size;
	  size_t occupied;
	  byte* occupied_mark;
	  // unsigned char* high_water_mark;
	  std::vector<byte*> indices;
	  Memory_Pool* Next_Memory_Pool  = nullptr;
	  bool has_next = false;
	  bool is_dirty = true;
	};

	void init_pool(const Pool& pool, size_t size);
	void defragment_pool(Pool& pool);
	void return_segment_to_pool(Pool& pool, byte* thing);
	byte* alloc_memory_from_pool(const Memory_Pool& Pool, size_t size);






}

#endif