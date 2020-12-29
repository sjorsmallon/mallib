	// calculate average (cluster) position, move away from "closest neighbour", swarm behavior.
	//@Dependencies:
	// g_wanted_distance; 
	// g_wanted_height;
	void evaluate_flying_units(Entity_Manager& entity_manager, const float dt)
	{
		timed_function("evaluate_flying_units");
		auto& player = g_player_entity;

		glm::vec3 position_sum{};
		size_t entity_count = 0;
		for (auto&& entity: by_type(entity_manager, Entity_Type::Cube))
		{
			position_sum += entity.position;
			entity_count += 1;
		}

		if (entity_count == 0)
		{
			// logr::report_warning("[evaluate_flying_units] no units found.\n");
			return;
		}

        //@Note(Sjors): be wary of div 0. The check above mitigates it.
        glm::vec3 average_position = position_sum *  (1.0f / static_cast<float>(entity_count));
        glm::vec3 player_position  = player.position + glm::vec3(0.0f,10.0f,0.0f);
        glm::vec3 cluster_center_to_player_direction = glm::normalize(player_position - average_position);
		
		struct Neighbour_Info
		{
			glm::vec3 direction;
			float distance;
		};

		//@Memory(Sjors)
		std::vector<Neighbour_Info> neighbour_info{};

		//@Speed(Sjors): O(N^2)
		// // wat we nu gaan doen, kost heel veel tijd.
		// // gather information about closest neighbour.
		for (auto&& lhs_e: by_type(entity_manager, Entity_Type::Cube))
		{
			Neighbour_Info neighbour{};
			neighbour.distance =  1000000.0f;
			neighbour.direction = glm::vec3(0.0f);

			for (auto&& rhs_e: by_type(entity_manager, Entity_Type::Cube))
			{
				// skip distance to self.
				if (rhs_e.id == lhs_e.id) continue;

				//@FIXME(Sjors): if position == position, distance becomes NaN.
				glm::vec3 distance  = rhs_e.position - lhs_e.position; 

				float abs_distance = sqrt(distance.x * distance.x + distance.y * distance.y + distance.z * distance.z);
				// float abs_distance = glm::length(distance);
				glm::vec3 direction_vector = glm::normalize(distance);

				if (abs_distance < neighbour.distance)
				{
					neighbour.distance = abs_distance;
					neighbour.direction = direction_vector;	
				}
			}
			neighbour_info.push_back(neighbour);
		}

		size_t neighbour_idx = 0;
		// update the positions of each thing.
		for (auto&& entity: by_type(entity_manager, Entity_Type::Cube))
		{
			auto& neighbour = neighbour_info[neighbour_idx]; 

			glm::vec3 focus_direction      = glm::normalize(player_position - entity.position);
			glm::vec3 cohesion_direction   = glm::normalize(average_position - entity.position);
			glm::vec3 separation_direction = -neighbour.direction;
			glm::vec3 height_direction     = glm::vec3(0.f, 1.f, 0.f); 

			glm::vec3 direction_vector(0.0f);
			bool enable_cohesion = true;
			bool enable_height = false;
			bool enable_focus = false;
			bool enable_separation = true;
			bool enable_center_to_player = true;
			bool enable_previous_momentum = false;

			if (enable_cohesion)	direction_vector += cohesion_direction * g_cohesion;
			if (enable_height)		direction_vector += height_direction;

		
			if (enable_separation )
			{
				if (neighbour.distance < g_wanted_distance)
				{
					direction_vector = separation_direction;
				}
			}

			if (enable_focus)				direction_vector += focus_direction;
			if (enable_center_to_player)	direction_vector += cluster_center_to_player_direction;

			direction_vector = glm::normalize(direction_vector);
			
			if (enable_previous_momentum)
			{
				glm::vec3 old_direction_vector = entity.movement_vector;
				direction_vector = glm::normalize(0.1f * direction_vector + 0.9f * old_direction_vector);
			}	

			if (entity_count == 1) direction_vector = focus_direction;

			entity.position = entity.position + (direction_vector * g_dodecahedron_velocity * dt);	
			entity.movement_vector = direction_vector;

			neighbour_idx += 1;
		}
 	}