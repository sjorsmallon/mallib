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


template<size_t bucket_size>
struct Entity_Manager
{
	using bucket = Bucket<Entity, bucket_size>;
	using bucket_array = Bucket_Array<Entity, bucket_size>;

	bucket_array array;
	std::map<Entity_Type, std::vector<bucket*>> per_type; // hmm. this should be an iterator?

};


create_entity(Entity_Manager& entity_manager, Entity_Type entity_type)
{
	
	
}



#endif


