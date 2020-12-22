#include "game.h"

#include <glm/gtx/string_cast.hpp>

#include "camera.h"
#include "input.h"
#include "logr.h"
#include "math.h"
#include "entity_manager.h"
#include "sound_system.h"
#include "timed_function.h"
#include <numeric> // for accumulate
#include <chrono>


// these are the GLFW key presses.
constexpr const int KEY_SPACE = 32;
constexpr const int KEY_1 = 48;
constexpr const int KEY_2 = 49;
constexpr const int KEY_3 = 50;
constexpr const int KEY_4 = 51;
constexpr const int KEY_5 = 52;
constexpr const int KEY_6 = 53;
constexpr const int KEY_7 = 54;
constexpr const int KEY_8 = 55;
constexpr const int KEY_9 = 56;

constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;
constexpr const int KEY_P = 80;
constexpr const int KEY_O = 79;
constexpr const int KEY_I = 73;

constexpr const float FRAMETIME_144_FPS = 0.006944444444f;
constexpr const float FRAMETIME_120_FPS = 0.00833333333f;
constexpr const float FRAMETIME_1000_FPS = 0.001f;
constexpr const float FRAMETIME_10_FPS = 0.1f;
constexpr const float FRAMETIME_IN_S = FRAMETIME_144_FPS;
constexpr const float ENGINE_HZ = 120.f; 

namespace
{

	//@FIXME(Sjors): to be replaced by a type_player entity.
	struct Player
	{
		uint32_t ID;
		glm::vec3 position;
		glm::vec3 movement_vector;
	};

	// player entity fields.
   	Player g_player_entity{0, glm::vec3{0.0f,0.0f,3.0f}, glm::vec3{0.0f,0.0f,0.0f}};

    // ---------------
	// cvars
	// ---------------

    // world
    float g_mouse_sensitivity = 0.08f;
    float g_camera_velocity = 0.2f;
 	float g_player_gravity = 50.f;
 	// glm::vec3 world_up?

    // player movement (these are adjusted by dt)
    float pm_jump_acceleration  = 35.f;
    float pm_ground_acceleration = 10.f;
    float pm_stopspeed = 100.f;
    float pm_maxspeed = 300.0f; // ground
    float pm_friction = 6.f;

    
    float pm_air_acceleration = 1.0f;
    

    float pm_maxairspeed = 400.f; // air
    float pm_air_deceleration = 0.1f;

    // flying units
    float g_dodecahedron_velocity = 70.0f;
    float g_wanted_distance = 80.0f;
    float g_wanted_height = 50.0f;
    float g_focus = 1.f;
    float g_alignment = 1.f;
    float g_cohesion = 1.f;

    [[nodiscard]]
	glm::vec3 apply_friction(glm::vec3 old_movement_vector, bool grounded, bool jump_pressed_this_frame, float dt)
	{
		old_movement_vector.y = 0.0f;
		float velocity = glm::length(old_movement_vector);

		const float velocity_treshold = 0.0000001f;
	
		if (velocity < velocity_treshold)
		{
			return glm::vec3(0.0f);
		}

		float velocity_drop = 0.0f;
		// if (!jump_pressed_this_frame)
		// {
		// 	if (grounded) // if grounded, we experience friction. 
		// 	{ 
		// 		float drop_base = 0.0f;
		// 		// if we are moving slower than the stopspeed, base decrement on deceleration.
		// 		// if (velocity < 120300);
		// 		// {
		// 		// 	drop_base = pm_stopspeed;
		// 		// } 
		// 		// else  // if we are moving faster than the acceleration, base decrement on own velocity (which will be harsher)
		// 		{
		// 			drop_base = velocity;
		// 		}
		// 		velocity_drop = drop_base * pm_friction * dt;
		// 	} 
		// }


		// adjust the velocity with the induced velocity drop. 
		float drop_adjusted_velocity = velocity - velocity_drop;
		if(drop_adjusted_velocity < 0.0f) drop_adjusted_velocity = 0.0f;
		float new_velocity = 0.0f;

		// normalize the velocity based on the ground velocity.
		if(drop_adjusted_velocity > 0.0f)
		{
			new_velocity = drop_adjusted_velocity;
			if (velocity > 1.0f) new_velocity /= velocity;
		}

		glm::vec3 adjusted_movement_vector = glm::normalize(old_movement_vector);
		return adjusted_movement_vector * new_velocity;
	}
	
	[[nodiscard]]
	glm::vec3 accelerate(glm::vec3 adjusted_movement_vector, glm::vec3 wish_direction, float wish_velocity, float acceleration, float dt)
	{
		logr::report("[acceleration]\n");

		float adjusted_velocity = glm::length(adjusted_movement_vector);
		logr::report("adjusted_velocity: {}\n", adjusted_velocity);

		logr::report("adjusted_movement_vector: {}\n", adjusted_movement_vector);
		logr::report("wish_direction: {}\n", wish_direction);
		float current_speed_in_wish_dir = glm::dot(adjusted_movement_vector, wish_direction);
		logr::report("current_speed_in_wish_dir: {}\n", current_speed_in_wish_dir);

		logr::report("wish_direction * adjusted_velocity: {}\n", wish_direction * adjusted_velocity);

		
		float acceleration_factor = acceleration;
		glm::vec3 acceleration_vector = dt * wish_direction;
		glm::vec3 old_heading = adjusted_movement_vector;

		glm::vec3 result = old_heading * dt + acceleration_factor *  acceleration_vector;

		glm::vec3 result_dir = glm::normalize(result_dir);
		float result_velocity = glm::length(result);

		if (adjusted_velocity < 0.0000001f)
		{
			logr::report("returning acceleration vector!\n");
			return acceleration_vector;
		}

		float max_speed = (acceleration == pm_ground_acceleration) ?  pm_maxspeed : pm_maxairspeed;
		
		// if (glm::length(result) > (max_speed * dt))
		// {
		// 	logr::report("velocity of result: {}\n", glm::length(result));
		// 	result = glm::normalize(result) * (max_speed * dt);
		// }	

		return result;
	}	

    //@Dependencies:
    // g_player_movespeed
    // g_player_acceleration
    // world_up
	std::tuple<glm::vec3, glm::vec3> update_position_with_input(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 old_movement_vector,
		const glm::vec3 front,
		const glm::vec3 right,
		const float dt)
	{
		static bool grounded = true;
		static float hang_time = 0.0f;
		bool jump_pressed_this_frame = input.keyboard_state[KEY_SPACE];	

		glm::vec3 adjusted_movement_vector = apply_friction(old_movement_vector, grounded, jump_pressed_this_frame, dt);



		float Y_old_y = old_movement_vector.y;

		const glm::vec3 plane_front = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
		const glm::vec3 plane_right = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

		glm::vec3 input_vector = glm::vec3(0.0f,0.0f,0.0f);

		if (input.keyboard_state[KEY_W]) input_vector += plane_front;
		if (input.keyboard_state[KEY_S]) input_vector -= plane_front;
		if (input.keyboard_state[KEY_A]) input_vector -= plane_right;
		if (input.keyboard_state[KEY_D]) input_vector += plane_right;

		bool received_input = (input.keyboard_state[KEY_W] ||
							   input.keyboard_state[KEY_S] ||
							   input.keyboard_state[KEY_A] ||
							   input.keyboard_state[KEY_D]);

		float input_velocity = 0.0f;
		// IF WE HAVE RECEIVED INPUT, NORMALIZE IT.
		if (received_input)
		{
			// NORMALIZE INPUT DIRECTION (WHAT HAPPENS IF THIS IS ALL ZEROES?)
			input_vector = glm::normalize(input_vector);
			input_velocity = 1.0f;
		} 

		float Y_input_y_velocity = 0.0f;
		if (input.keyboard_state[KEY_SPACE])
		{
			if (grounded)
			{
				if (received_input)		input_velocity *= 5.f;

				hang_time = 0.0f;
				grounded = false;
				jump_pressed_this_frame = true;
			}
		}

		glm::vec3 movement_vector = glm::vec3(0.0f);
		// IF NO INPUT IS RECEIVED, CONTINUE IN THE SAME DIRECTION WITH SAME SPEED AS WE WERE ON.
		if (input_velocity < 0.1f) 
		{
			// don't normalize: will yield nan or inf
			movement_vector = adjusted_movement_vector;
		}
		else
		{
			// POSSIBLY USE DIFFERENT ACCELERATION VALUES FOR WHEN WE ARE IN THE AIR.
			float acceleration = (grounded) ? pm_ground_acceleration : pm_air_acceleration;
			movement_vector = accelerate(adjusted_movement_vector, input_vector, input_velocity, acceleration, dt);
		}

		movement_vector.y  = Y_old_y;
		movement_vector.y += Y_input_y_velocity;

		//Y
		if (!grounded)
		{
			hang_time += dt;
			float x = hang_time;
			float c = 0.0f;
			float b = pm_jump_acceleration;
			float a = -g_player_gravity;
			movement_vector.y = a * x * x + b * x + c; 
		}
		
		// try to move (collide with ground plane etc etc etc)
		glm::vec3 position = glm::vec3(old_position.x, 0.0f, old_position.z) + movement_vector;
		// // clip the movement vector.
		if (position.y < 0.0f)
		{
			grounded = true;
			position.y = 0.0f;
			movement_vector.y = 0.0f;
			hang_time = 0.0f;
		}
		
		return std::make_tuple(position, movement_vector);
	}

    // @dependencies:
    // g_camera_velocity
	glm::vec3 update_flying_camera_with_input(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 front,
		const glm::vec3 right,
		const glm::vec3 up,
		const float dt)
	{
		glm::vec3 position = old_position;

		if (input.keyboard_state[KEY_W]) position = position + (front * g_camera_velocity * dt);
		if (input.keyboard_state[KEY_S]) position = position - (front * g_camera_velocity * dt);
		if (input.keyboard_state[KEY_A]) position = position - (right * g_camera_velocity * dt);
		if (input.keyboard_state[KEY_D]) position = position + (right * g_camera_velocity * dt);

		if (input.keyboard_state[KEY_SPACE]) position = position + (up * g_camera_velocity * dt);

		return position;
	}

	// processes input received from a mouse input system.
	// assumes world up direction is positive y.
	// @dependencies: 
	// g_mouse_sensitivity
	Camera update_camera_view_with_input(const Input& input, const Camera camera, const float dt, const bool should_constrain_pitch = true)
	{
		const float pitch_constrain_treshold_degrees = 89.0f;

		Camera new_camera = camera;
	    glm::vec3 world_up(0.0f,1.0f, 0.0f);

	    float adjusted_x_offset = static_cast<float>(input.mouse_delta_x) * g_mouse_sensitivity;
	    float adjusted_y_offset = static_cast<float>(input.mouse_delta_y) * g_mouse_sensitivity;

	    new_camera.yaw   += adjusted_x_offset;
	    new_camera.pitch += adjusted_y_offset;

	    // make sure that when pitch is out of bounds, screen doesn't get flipped
	    if (should_constrain_pitch)
	    {
	        if (new_camera.pitch > pitch_constrain_treshold_degrees)  new_camera.pitch = pitch_constrain_treshold_degrees;
	        if (new_camera.pitch < -pitch_constrain_treshold_degrees) new_camera.pitch = -pitch_constrain_treshold_degrees;
	    }

	    // update front, right and up Vectors using the updated euler angles
	    glm::vec3 front{};
	    front.x = cos(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
	    front.y = sin(glm::radians(new_camera.pitch));
	    front.z = sin(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
	    new_camera.front = glm::normalize(front);
	    
	    // also re-calculate  & normalize the right and up vector
	    new_camera.right = glm::normalize(glm::cross(new_camera.front, world_up)); 
	    new_camera.up    = glm::normalize(glm::cross(new_camera.right, new_camera.front));

	    return new_camera;
	}

	Camera update_camera_entity(const Input& input, const Camera old_camera, const float dt)
	{
		Camera camera = old_camera;
		auto [position, movement_vector] = update_position_with_input(input, old_camera.position, old_camera.movement_vector, old_camera.front, old_camera.right, dt); 
		camera.position = position;
		camera.movement_vector = movement_vector;

		if (input.mouse_delta_x || input.mouse_delta_y)
			return update_camera_view_with_input(input, camera, dt);

		return camera;
	}


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

	void evaluate_shot(Entity_Manager& entity_manager, Camera camera)
	{
		timed_function("evaluate_shot");
		const float sphere_radius = 20.f;

		for(auto&& entity: by_type(entity_manager, Entity_Type::Cube))
		{
  			if (!ray_intersects_sphere(camera.position, camera.front, entity.position, sphere_radius)) continue;

  			// we hit!
			schedule_for_destruction(entity_manager, &entity);
		}
	}

}

void game_simulate(Game_State& game_state, const double dt, const Input& input, Particle_Cache& particle_cache, Entity_Manager& entity_manager)
{
	// maybe clamp dt
	float clamped_dt = static_cast<float>(dt);	
	if (clamped_dt < FRAMETIME_1000_FPS)	clamped_dt = FRAMETIME_1000_FPS;	
	if (clamped_dt > FRAMETIME_10_FPS)		clamped_dt = FRAMETIME_10_FPS;  	

	// process non-game input
	{
		if (input.keyboard_state[KEY_P]) game_state.game_mode = GM_GAME;
		if (input.keyboard_state[KEY_O]) game_state.game_mode = GM_EDITOR;
		if (input.keyboard_state[KEY_I]) game_state.paused = 1 - game_state.paused; // toggle
	}

	// are we paused?
	if (game_state.paused)
	{
		// paused!
	}
	else
	{
		// if (game_state.game_mode == GM_GAME)
		// {
			// BEFORE MOVING ANYTHING, check shot intersection, since we clicked at the position we _are_ in
			// and target that _are _ in a particular position
			if (input.mouse_left) evaluate_shot(entity_manager, game_state.camera);

			//@Note(Sjors): it is imperative that the player gets updated first, since that 
			// is the bottleneck we have to be dealing with in the next frames. (after, we can go wide).

			// update player and camera.
			{
				game_state.camera = update_camera_entity(input, game_state.camera, clamped_dt);
				g_player_entity.position = game_state.camera.position;
			}

			 // update dodecahedrons
			{
				evaluate_flying_units(entity_manager, dt);
			}

			{
				static bool played = false;
				if (!count_by_type(entity_manager, Entity_Type::Cube)) 
				{
					if (!played) play_sound("applause");
					played = true;
				}
			}

			// at this point, we can start rendering static geometry.
			// collision_check()

			// update sound system
			{
				update_listener(
					g_player_entity.position.x,
					g_player_entity.position.y,
					g_player_entity.position.z,
					game_state.camera.front.x,
					game_state.camera.front.y,
					game_state.camera.front.z,
					game_state.camera.up.x,
					game_state.camera.up.y,
					game_state.camera.up.z,
					game_state.camera.movement_vector.x,
					game_state.camera.movement_vector.y,
					game_state.camera.movement_vector.z
				);
			
				if (input.mouse_right) play_sound_3d("chicken");
				if (input.mouse_left)  play_sound("plop_shorter_runup");
			}

		// }
	}
}


// call once
void game_init()
{


}
