#ifndef _TEST_ENTITY_CONTAINER_
#define _TEST_ENTITY_CONTAINER_
#include "bucket_array.h"
#include <map>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


using entity_id = uint32_t; 
using generation = uint32_t;
using Mesh_id = size_t;

enum Entity_Type : uint32_t
{
	None = 0,
	Player = 1,
	Cube = 2,
	COUNT = 3
};

struct Xform_State
{
	glm::vec3 position;
	glm::vec4 orientation;
	float scale;
};

// don't declare any virtual functions please.
struct Entity
{
    // things we thought about
    entity_id id;
    Entity_Type type;
    glm::vec3 position;
	glm::vec3 movement_vector;

    // things we need to decide on
    // std::string mesh_name;
    generation generation; // this should actually be a counter for this particular entity ID (i.e. what generation is this?) instead of for this particular entity.
    bool scheduled_for_destruction; // = false
    bool visible; // = false;
    Mesh_id mesh_id;
    Xform_State xform_state;
};




static const int bucket_size = 100;
using bucket = Bucket<Entity, bucket_size>;
using bucket_array = Bucket_Array<Entity, bucket_size>;

struct Entity_Manager
{
	bucket_array all_buckets;
	std::map<Entity_Type, std::deque<bucket*>> buckets_by_type; // hmm. this should be an iterator?
};



//@ Me tomorrow: see if we can't use iterators instead... this is kind of ugly.


//@Note(Sjors): returns null for invalid.   // haha bucket list
inline Entity* next_entity(std::deque<bucket*>& bucket_list, size_t& current_bucket_idx, size_t& current_entity_idx)
{
	bucket* current_bucket = bucket_list[current_bucket_idx];

	current_entity_idx = current_entity_idx + 1;
	if (current_entity_idx >= current_bucket->capacity)
	{
		if (current_bucket_idx < bucket_list.size() - 1)
		{
			current_bucket_idx += 1;
			current_bucket = bucket_list[current_bucket_idx];
			current_entity_idx = 0;
		}
		else
		{
			return nullptr;
		}
	}	
	return &current_bucket->data[current_entity_idx];
}


// #define for_entity_by_type_macro(entity_manager, entity_type) \
// size_t current_bucket = 0; \
// size_t current_idx = 0; \
// while (true) \
// { \
// 	Entity *e_ptr = next_entity(entity_manager.buckets_by_type[entity_type], current_bucket, current_entity); \
// 	if (entity == nullptr) break; \
// 	Entity& entity = entity_ptr; \




inline void create_entity(Entity_Manager& entity_manager, Entity_Type entity_type)
{
	if (!entity_manager.buckets_by_type[entity_type].size())
		add_bucket(entity_manager.all_buckets);

	auto&& bucket = get_last_bucket(entity_manager.all_buckets);

	Entity entity{};
	entity.type = entity_type;
	entity.xform_state = {glm::vec3{0.f,2 * entity.id, 0.f}, glm::vec4(0.f), 1.f};
	entity.position =    glm::vec3{0.f, 2 * entity.id, 0.f};

	array_add(entity_manager.all_buckets, std::move(entity));


	entity_manager.buckets_by_type[entity_type].push_back(bucket);

	// bucket_add(bucket, std::move(entity));
}

// inline void add_entity(Entity_Manager& entity_manager, Entity& entity)
// {
	// if (!entity_manager.buckets_by_type[entity_type].size())
	// 		add_bucket(entity_manager.all_buckets);

	// array_add(entity_manager.all_buckets, entity);

	// if (entity_manager.per_type[entity.type].size())	add_bucket(entity_manager.all_buckets);
	// auto&& bucket = get_last_bucket(entity_manager.all_buckets);

	// array_add(bucket, entity);

// }



#endif


