#ifndef INCLUDED_ENTITY_
#define INCLUDED_ENTITY_
#include "../xform_state/xform_state.h" 

namespace entity
{
	enum class Duration
	{
		INFINITE,
		ON_TIMER
	};


	// does every entity have a xform_state?
	// or is that a waste?
	// or do we want some kind of component system?
	struct Entity
	{	
		//@Refactor: this used to be position. Can we get away with this?
		Xform_State xform_state;
		// Vec3 position;		
		uint32_t id = 0; 
		bool hidden;
		bool marked_for_destruction; // destroy this entity next frame.
		Duration duration;

		// std::string debug_notes;  (for debugging purposes / inspection of entities.)
	};

	// the entity manager has a non-owning pointer to a list of entities.
	// or should it own the entity list?
	class Entity_Manager()
	{
		 std::vector<Entity>* entities = nullptr;

			public:
				Entity_Manager() = default;
				~Entity_Manager() = default;
				Entity_Manager(const Entity_Manager& em) = delete; // no copying
				Entity_Manager(Entity_Manager&& em) = delete;  // no moving
				void operator=(const Entity_Manager& em) = delete; // no copying through assignment

				// @Refactor: we could pass this in the constructor actually.
				// is there any reason not to?
				void set_entity_pointer(std::vector<Entity>* entities);
	}	

	uint32_t register_entity(Entity&& e) // always move, never fear 
	{
		// how do we want to do entity system?
	}
}






struct Scene
{

}


// lifetime component?

#endif