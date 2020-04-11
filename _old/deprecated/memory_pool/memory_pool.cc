#include "memory_pool.h"
#include "fmt/core.h"


void mem::init_pool(mem::Pool& pool, size_t size)
{
	  pool.data = (byte*)malloc(size); 
	  if (pool.data == nullptr)
	  	fmt::print("mem::init_pool: malloc returns null. can't handle so ungracefully shutting down.\n");

	  pool.size = size;
	  pool.occupied 0;
	  pool.occupied_mark = pool.data;
	  pool.Next_Memory_Pool = nullptr;
	  pool.has_next = false;
	  pool.is_dirty = false;
	  // weak_ptr refcount? or what?
}

void return_segment_to_pool(mem::Pool& pool, byte* thing)
{
	//@TODO: we need to remove it from the array / vector/ map / whatever we do.
	// think about things like: if the size of the free thing was big,
	// do we want to reshuffle?
	// but what happens when we reshuffle? addresses get invalidated.
	// so maybe we don't want to work with raw addresses but interface
	// through some kind of lookup table.
	// furthermore, we need to keep track of all of the addresses that we hand out,
	// so that when we get it back, we can reshuffle.
}

unsigned char* mem::alloc_memory_from_pool(const mem::Pool& pool, size_t size) // size in bytes! i.e. the result of sizeof
{
	if (pool.occupied + size < pool.size)
	{
		// good. we have enough space.
		byte* new_occupied_mark = pool.occupied_mark += size;
		pool.indices.push_back(pool.occupied_mark);
		return pool.occupied_mark;
	}
	else
	{
		//@FIXME: this sucks.
		fmt::print("ALLOC_MEMORY_FROM_POOL: NOT ENOUGH SPACE.@FIXME\n");
	}
}