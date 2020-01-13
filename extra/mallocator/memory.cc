#include "memory.h"

size_t& mem::active_memory()
{
	static size_t active_memory;
	return active_memory;
}