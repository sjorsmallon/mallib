#include "texture_manager.h"

/// returns a free active texture ID and increments the value.
uint32_t tm::next_free_active_texture_ID(Texture_Manager& texture_manager)
{
	 return manager.next_texture_id++;
}
