#ifndef INCLUDED_ENTITY_
#define INCLUDED_ENTITY_

enum class lifetime
{
	ON_TIMER,
	INFINITE,
};

struct Entity
{
	bool marked_for_destruction;
	vec::Vec3 position;
	vec::Vec3 
};

// lifetime component?

#endif