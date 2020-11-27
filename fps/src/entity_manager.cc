#include "entity_manager.h"
#include "log.h"

namespace
{

	uint32_t get_next_entity_id(Entity_Manager& entity_manager)
	{
		return entity_manager.next_entity_id++;
	}

	Entity& get_or_create_entity(Entity_Manager& entity_manager, Entity_Type type)
	{
		uint32_t entity_idx = 0;
		uint32_t entity_id = 0;

		// do we have a free idx?
		if (entity_manager.free_indices[type].size())
		{
			entity_idx = entity_manager.free_indices[type].front();
			entity_manager.free_indices[type].pop_front();
		}
		else
		{
			entity_idx = entity_manager.entities[type].size();
			entity_manager.entities[type].push_back(Entity{0});
		}

		// do we have a free id?
		if (entity_manager.free_ids.size())
		{
			entity_id = entity_manager.free_ids.front();
			entity_manager.free_ids.pop_front();
		}	
		else
		{
			entity_id = get_next_entity_id(entity_manager);
		}

		Entity* entity = &entity_manager.entities[type][entity_idx];
		entity->id = entity_id;


		return (*entity);
	}
}

void create_entity(Entity_Manager& entity_manager, Entity_Type type)
{
	Entity& entity = get_or_create_entity(entity_manager, type);
	entity.type = type;
	entity.mesh_name = "dodecahedron";
	entity.xform_state = {glm::vec3{0.f,2 * entity.id, 0.f}, glm::vec4(0.f), 1.f};
	entity.position =    glm::vec3{0.f, 2 * entity.id, 0.f};
}
void make_entities_active(Entity_Manager& entity_manager)
{
	for (auto&& entity: entity_manager.entities[Entity_Type::Cube])
	{
		entity_manager.active_entities[Entity_Type::Cube].push_back(&entity);
	}
}

std::vector<Entity>& by_type(Entity_Manager& entity_manager, Entity_Type type)
{
	return entity_manager.entities[type]; 
}

std::vector<Entity*>& by_type_ptr(Entity_Manager& entity_manager, Entity_Type type)
{
	return entity_manager.active_entities[type];
}

// std::vector<Entity*> by_type(Entity_Manager& entity_manager, Entity_Type type)
// {
// 	logr::report_warning_once("[entity_manager] by_type: implementation unfinished!\n");
// 	std::vector<Entity*> result;
// 	for (auto& entity: entity_manager.entities[type])
// 		result.push_back(&entity);


// 	return result; 
// }