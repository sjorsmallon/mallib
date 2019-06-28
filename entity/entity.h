#ifndef INCLUDED_ENTITY_
#define INCLUDED_ENTITY_

enum class lifetime
{
	ON_TIMER,
	INFINITE,
}

struct Entity
{
	bool marked_for_destruction;
	
};

// lifetime component?

#endif