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

// these are the GLFW key presses.
constexpr const int KEY_SPACE = 32;
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
 	float g_player_gravity = 0.01f;

 	// calibrated for 120hz, but vsync is 144hz. woops.
 	// but that shouldn't actually matter right?

    // player movement
    float pm_jump_acceleration   = 1.0f;
    float pm_ground_acceleration = 0.166666666f;
    float pm_ground_deceleration =  1.66666666f;
    float pm_friction = 0.1f;

    float pm_air_deceleration = 0.1f;
  
    // flying units
    float g_dodecahedron_velocity = 1.f;
    float g_wanted_distance = 80.0f;
    float g_wanted_height = 50.0f;
    float g_focus = 1.f;
    float g_alignment = 1.f;
    float g_cohesion = 1.f;



   
	glm::vec3 apply_friction(glm::vec3 old_movement_vector, bool grounded, float dt_factor)
	{
		logr::report("apply_friction\n");
		if (grounded) old_movement_vector.y = 0.0f;	


		float velocity = glm::length(old_movement_vector);

		if (velocity < 0.0000001f)
		{
			return glm::vec3(0.0f);
		}

		float velocity_drop = 0.0f;
		bool jump_pressed_this_frame = false;
		if (!jump_pressed_this_frame)
		{
			if (grounded) // if grounded, we experience friction. 
			{ 
				// if we are moving slower than the acceleration, base decrement on deceleration.
				// if we are moving faster than the acceleration, base decrement on own velocity (which will be harsher)
				float drop_base = (velocity < pm_ground_acceleration) ? pm_ground_deceleration : velocity;
				velocity_drop = drop_base * pm_friction * dt_factor;
			} 
		}
		// adjust the velocity with the induced velocity drop. 
		float drop_adjusted_velocity = velocity - velocity_drop;
		if(drop_adjusted_velocity < 0.0f) drop_adjusted_velocity = 0.0f;

		// normalize the velocity based on the ground velocity.
		if(drop_adjusted_velocity > 0.0f)
		{
			drop_adjusted_velocity /= velocity;
		}
		
		float new_velocity = drop_adjusted_velocity;
		glm::vec3 adjusted_movement_vector = glm::normalize(old_movement_vector);
		logr::report("velocity: {}\n", velocity);
		logr::report("drop_adjust_velocity: {}\n", drop_adjusted_velocity);
		logr::report("apply_friction movement_vector: {}\n", glm::to_string(adjusted_movement_vector));

		return adjusted_movement_vector * new_velocity;
	}

	glm::vec3 accelerate(glm::vec3 old_movement_vector, glm::vec3 wish_direction, float wish_velocity, float acceleration, float dt_factor)
	{
		float current_speed = glm::dot(old_movement_vector, wish_direction);
		float delta_speed = wish_velocity - current_speed;
		
		if (delta_speed <= 0.0f) return glm::vec3(0.0f);

		float accel_speed = acceleration *  wish_velocity * dt_factor;
		if (accel_speed > delta_speed) accel_speed = delta_speed;

		logr::report("accelerate old movement_vector:{}\n", glm::to_string(old_movement_vector));
		logr::report("wish_direction: {}\n", glm::to_string(wish_direction));
		logr::report("wish_velocity: {}\n", wish_velocity);
		logr::report("current_speed: {}\n", current_speed);
		logr::report("delta_speed: {}\n", delta_speed);
		logr::report("accel_speed: {}\n", accel_speed);
		glm::vec3 result = old_movement_vector + accel_speed * wish_direction;
		logr::report("result: {}\n", glm::to_string(result));

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
		const float dt_factor)
	{
		static bool grounded = true;
		bool jump_pressed_this_frame = false;
		logr::report("start: old movement_vector:{}\n", glm::to_string(old_movement_vector));
		glm::vec3 adjusted_movement_vector = apply_friction(old_movement_vector, grounded, dt_factor);
		// in very fancy terms: "project forward and right to flat plane"

		const glm::vec3 plane_front = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
		const glm::vec3 plane_right = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

		glm::vec3 input_vector = glm::vec3(0.0f,0.0f,0.0f);

		float input_y_velocity = 0.0f;
		if (input.keyboard_state[KEY_W]) input_vector += plane_front;
		if (input.keyboard_state[KEY_S]) input_vector -= plane_front;
		if (input.keyboard_state[KEY_A]) input_vector -= plane_right;
		if (input.keyboard_state[KEY_D]) input_vector += plane_right;
		if (input.keyboard_state[KEY_SPACE])
		{
			if (grounded)
			{
				input_y_velocity += pm_jump_acceleration; 
				grounded = false;
				jump_pressed_this_frame = true;
			}
		}

		float input_velocity = glm::length(input_vector);
	

		if (input_velocity < 0.0001f) 
		{
			// don't normalize: will yield nan or inf			
		}
		else
		{
			input_vector = glm::normalize(input_vector);
		}

		
		//@IC(Sjors): DO NOT forget the brackets here.
		float acceleration = (grounded) ? pm_ground_acceleration : pm_air_deceleration;
		glm::vec3 movement_vector = accelerate(adjusted_movement_vector, input_vector, input_velocity, acceleration, dt_factor);
		// at this point, movement_vector is adjusted for dt.
		// logr::report("movement_vector:{}\n", glm::to_string(movement_vector));
		movement_vector.y += input_y_velocity;

		if (old_position.y > 0.0f) movement_vector.y -= g_player_gravity * dt_factor;
		float velocity = glm::length(movement_vector);


		// try to move (collide with ground plane etc etc etc)

		glm::vec3 position = old_position + movement_vector;
		// // clip the movement vector.
		if (position.y < 0.0f)
		{
			grounded = true;
			position.y = 0.0f;
			movement_vector.y = 0.0f;
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
		const float dt_factor)
	{
		glm::vec3 position = old_position;

		if (input.keyboard_state[KEY_W]) position = position + (front * g_camera_velocity * dt_factor);
		if (input.keyboard_state[KEY_S]) position = position - (front * g_camera_velocity * dt_factor);
		if (input.keyboard_state[KEY_A]) position = position - (right * g_camera_velocity * dt_factor);
		if (input.keyboard_state[KEY_D]) position = position + (right * g_camera_velocity * dt_factor);

		if (input.keyboard_state[KEY_SPACE]) position = position + (up * g_camera_velocity * dt_factor);

		return position;
	}

	// processes input received from a mouse input system.
	// assumes world up direction is positive y.
	// @dependencies: 
	// g_mouse_sensitivity
	Camera update_camera_view_with_input(const Input& input, const Camera camera, const float dt_factor, const bool should_constrain_pitch = true)
	{
		Camera new_camera = camera;
	    glm::vec3 world_up(0.0f,1.0f, 0.0f);

	    float adjusted_x_offset = input.mouse_delta_x * g_mouse_sensitivity * dt_factor;
	    float adjusted_y_offset = input.mouse_delta_y * g_mouse_sensitivity * dt_factor;

	    new_camera.yaw   += adjusted_x_offset;
	    new_camera.pitch += adjusted_y_offset;

	    // make sure that when pitch is out of bounds, screen doesn't get flipped
	    if (should_constrain_pitch)
	    {
	        if (new_camera.pitch > 89.0f)  new_camera.pitch = 89.0f;
	        if (new_camera.pitch < -89.0f) new_camera.pitch = -89.0f;
	    }

	    // update front, right and up Vectors using the updated euler angles
	    glm::vec3 front;
	    front.x = cos(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
	    front.y = sin(glm::radians(new_camera.pitch));
	    front.z = sin(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
	    new_camera.front = glm::normalize(front);
	    
	    // also re-calculate the right and up vector
	    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	    new_camera.right = glm::normalize(glm::cross(new_camera.front, world_up)); 
	    new_camera.up    = glm::normalize(glm::cross(new_camera.right, new_camera.front));

	    return new_camera;
	}

	Camera update_camera_entity(const Input& input, const Camera old_camera, const float dt_factor)
	{
		Camera camera = old_camera;
		auto [position, movement_vector] = update_position_with_input(input, old_camera.position, old_camera.movement_vector, old_camera.front, old_camera.right, dt_factor); 
		camera.position = position;
		camera.movement_vector = movement_vector;

		if (input.mouse_delta_x || input.mouse_delta_x)
			return update_camera_view_with_input(input, camera, dt_factor);

		return camera;
	}

	void update_player_entity(const Input& input)
	{
		auto& player = g_player_entity;
	}

	void evaluate_flying_units(Entity_Manager& entity_manager, const float dt_factor)
	{
		auto& player = g_player_entity;
		auto& dodecahedrons = by_type(entity_manager, Entity_Type::Cube);

    	auto sum_lambda = [](glm::vec3 sum, Entity& rhs) {
                         return (sum + rhs.position);
                     	};
        glm::vec3 sum = std::accumulate(dodecahedrons.begin(), dodecahedrons.end(), glm::vec3(0.0f), sum_lambda);

        //@Incomplete(Sjors): div 0?
        glm::vec3 average_position = sum *  (1.0f/ static_cast<float>(dodecahedrons.size()));
        glm::vec3 player_position = player.position + glm::vec3(0.0f,10.0f,0.0f);
        glm::vec3 center_to_player_direction = glm::normalize(player_position - average_position);
		
		struct Neighbour_Info
		{
			glm::vec3 direction;
			float distance;
		};

		//@Memory
		std::vector<Neighbour_Info> neighbour_info(dodecahedrons.size());
		//@Speed(Sjors): O(N^2)
		// wat we nu gaan doen, kost heel veel tijd.
		// gather information about closest neighbour.
		for (size_t lhs_idx = 0; lhs_idx != dodecahedrons.size(); ++lhs_idx)
		{
			Entity& lhs_e = dodecahedrons[lhs_idx];
			auto& neighbour = neighbour_info[lhs_idx];
			neighbour.distance =  1000000.0f;
			neighbour.direction = glm::vec3(1.0f);

			for (size_t rhs_idx = lhs_idx + 1; rhs_idx != dodecahedrons.size(); ++rhs_idx)
			{
				auto& rhs_e = dodecahedrons[rhs_idx];

				glm::vec3 distance  = rhs_e.position - lhs_e.position; 
				float abs_distance = glm::length(distance);
				glm::vec3 direction_vector = glm::normalize(distance);

				if (abs_distance < neighbour.distance)
				{
					neighbour.distance = abs_distance;
					neighbour.direction = direction_vector;	
				}
			}
		}

		// update the positions of each thing.
		for (size_t idx = 0; idx != dodecahedrons.size(); ++idx)
		{
			Entity& entity = dodecahedrons[idx];
			auto& neighbour = neighbour_info[idx]; 

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

			if (enable_cohesion)
			{
				direction_vector += cohesion_direction * g_cohesion;
			}

			if (enable_height)
			{
				direction_vector += height_direction;
			}

			if (enable_separation)
			{
				if (neighbour.distance < g_wanted_distance)
				{
					direction_vector = separation_direction;
				}
			}

			if (enable_focus)
			{
				direction_vector += focus_direction;
			}

			if (enable_center_to_player)
			{
				direction_vector += center_to_player_direction;
			}

			direction_vector = glm::normalize(direction_vector);
			
			if (enable_previous_momentum)
			{
				glm::vec3 old_direction_vector = entity.movement_vector;
				direction_vector = glm::normalize(0.1f * direction_vector + 0.9f * old_direction_vector);
			}	



			entity.position = entity.position + (direction_vector * g_dodecahedron_velocity * dt_factor);	
			entity.movement_vector = direction_vector;
		}

		float frametime = dt_factor * FRAMETIME_IN_S;
		// logr::report("general entity distance this frame: {}\n", g_dodecahedron_velocity * dt_factor);
		// logr::report("over frametime: {}\n", (g_dodecahedron_velocity * dt_factor) / frametime);

	}

	void evaluate_shot(Entity_Manager& entity_manager, Camera camera)
	{
		timed_function("evaluate_shot");

		std::vector<Entity*>& entities = by_type_ptr(entity_manager, Entity_Type::Cube);
		for (Entity* entity_ptr: entities)
		{
  			if (!ray_intersects_sphere(camera.position, camera.front, entity_ptr->position, 20.0f)) continue;
			// we hit.
			bool mark_for_deletion = true;
			schedule_for_destruction(entity_manager, entity_ptr);
		}

	}



}

// I can't stress enough that we should absolutely NOT use dt.
// update and render world
void game_simulate(Game_State& game_state, const double dt, const Input& input, Particle_Cache& particle_cache, Entity_Manager& entity_manager)
{
	float clamped_dt = dt;	
	if (clamped_dt < FRAMETIME_1000_FPS) clamped_dt = FRAMETIME_1000_FPS; 
	if (clamped_dt > FRAMETIME_10_FPS) clamped_dt = FRAMETIME_10_FPS;  
	bool vsync = true;

	if (vsync) clamped_dt = FRAMETIME_144_FPS;
 
 	const float dt_factor = clamped_dt / FRAMETIME_144_FPS;

	// process higher level input
	{
		if (input.keyboard_state[KEY_P]) game_state.game_mode = GM_GAME;
		if (input.keyboard_state[KEY_O]) game_state.game_mode = GM_EDITOR;
		if (input.keyboard_state[KEY_I]) game_state.paused = 1 - game_state.paused; // toggle;
	}

	// are we paused?
	if (game_state.paused)
	{
		// paused!
	}
	else
	{
		// if game_mode = player_cam:
		{

			// BEFORE MOVING ANYTHING, check shot intersection?
			//if (input.mouse_left) evaluate_shot(entity_manager, game_state.camera);


			// post_shoot_reevaluate(entity_manager);
			//@Note(Sjors): it is imperative that the player gets updated first, since that 
			// is the bottleneck we have to be dealing with in the next frames.

			// update player and camera.
			{
				update_player_entity(input);
				game_state.camera = update_camera_entity(input, game_state.camera, dt_factor);
				g_player_entity.position = game_state.camera.position;
			}

			// // update dodecahedrons
			// {
			// 	evaluate_flying_units(entity_manager, dt_factor);
			// }

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
				if (input.mouse_left) play_sound("plop_shorter_runup.wav");
			}
		}
	}
}






// this is actually a really bad idea.
void game_init()
{

    // flying units
    // logr::add_float_input("g_dodecahedron_velocity", &g_dodecahedron_velocity, 0.0f, 1.0f);
    // logr::add_float_input("g_wanted_distance", &g_wanted_distance, 0.0f, 1.0f);
    // logr::add_float_input("g_wanted_height", &g_wanted_height, 0.0f, 1.0f);
    // logr::add_float_input("g_focus", &g_focus, 0.0f, 1.0f);
    // logr::add_float_input("g_alignment", &g_alignment, 0.0f, 1.0f);
    // logr::add_float_input("g_cohesion", &g_cohesion, 0.0f, 1.0f);

}
