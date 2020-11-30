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

 	// calibrated for 120hz, but vsync is 144hz. woops.
 	// but that shouldn't actually matter right?
    // player movement
    float pm_jump_acceleration  = 35.f;
    float pm_ground_acceleration = 10.f;
    float pm_stopspeed = 100.f;
    float pm_maxspeed = 100.0f; 
    float pm_friction = 6.f;

    float pm_air_acceleration = 11.0f;
    float pm_air_deceleration = 0.1f;
  
    // flying units
    float g_dodecahedron_velocity = 1.f;
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
	
		if (velocity < 0.0000001f)
		{
			return glm::vec3(0.0f);
		}

		float velocity_drop = 0.0f;
		if (!jump_pressed_this_frame)
		{
			if (grounded) // if grounded, we experience friction. 
			{ 
				float drop_base = 0.0f;
				// if we are moving slower than the stopspeed, base decrement on deceleration.
				// if (velocity < 120300);
				// {
				// 	drop_base = pm_stopspeed;
				// }
				// else  // if we are moving faster than the acceleration, base decrement on own velocity (which will be harsher)
				{
					drop_base = velocity;
				}
				velocity_drop = drop_base * pm_friction * dt;
			} 
		}

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

		// logr::report("velocity: {}\n", velocity);
		// logr::report("velocity_drop: {}\n", velocity_drop);

		// logr::report("drop_adjusted_velocity: {}\n", drop_adjusted_velocity);
		glm::vec3 adjusted_movement_vector = glm::normalize(old_movement_vector);
		return adjusted_movement_vector * new_velocity;
	}
	
	[[nodiscard]]
	glm::vec3 accelerate(glm::vec3 old_movement_vector, glm::vec3 wish_direction, float wish_velocity, float acceleration, float dt)
	{
		// logr::report("accelerate old movement_vector:{}\n", glm::to_string(old_movement_vector));
		float current_speed_in_wish_direction = glm::dot(old_movement_vector, wish_direction);

		// logr::report("current_speed: {}\n", current_speed_in_wish_direction);
		// logr::report("wish_direction: {}\n", glm::to_string(wish_direction));
		// logr::report("wish_velocity: {}\n", wish_velocity);

		// things to think about: since the wish direction is a unit vector,
		// delta speed will be HUGE when the timestep decreases.
		// does that matter?
		float delta_speed = wish_velocity - current_speed_in_wish_direction;
		float accel_speed = acceleration * wish_velocity * dt;
		// logr::report("delta_speed: {}\n", delta_speed);
		// logr::report("accel_speed: {}\n", accel_speed);


		if (delta_speed < 0.0f)
		{
		 	accel_speed = accel_speed;
		}

		if (accel_speed > delta_speed) accel_speed = delta_speed;

		glm::vec3 result = old_movement_vector + (accel_speed * wish_direction);

		
		// do not normalize if in the air.
		if (acceleration == pm_ground_acceleration)
		{
			if (glm::length(result) > (pm_maxspeed * dt))
			{
			result = glm::normalize(result) * (pm_maxspeed * dt);
			}	
		}

		

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
		static bool want_to_jump = false;
		static bool grounded = true;
		static std::chrono::time_point<std::chrono::system_clock> start{};
		static std::chrono::time_point<std::chrono::system_clock> end{};
		static float j_time = 0.0f;



		bool jump_pressed_this_frame = input.keyboard_state[KEY_SPACE];	

		float Y_old_y = old_movement_vector.y;
		logr::report("Y_old_y: {}\n", Y_old_y);

		glm::vec3 adjusted_movement_vector = apply_friction(old_movement_vector, grounded, jump_pressed_this_frame, dt);

		const glm::vec3 plane_front = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
		const glm::vec3 plane_right = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

		glm::vec3 input_vector = glm::vec3(0.0f,0.0f,0.0f);

		if (input.keyboard_state[KEY_W]) input_vector += plane_front;
		if (input.keyboard_state[KEY_S]) input_vector -= plane_front;
		if (input.keyboard_state[KEY_A]) input_vector -= plane_right;
		if (input.keyboard_state[KEY_D]) input_vector += plane_right;

		glm::vec3 position{};

		float input_velocity = glm::length(input_vector);

		// IF WE HAVE RECEIVED INPUT, NORMALIZE IT.
		if (input_velocity > 0.00001f)
		{
			// NORMALIZE INPUT DIRECTION (WHAT HAPPENS IF THIS IS ALL ZEROES?)
			input_vector = glm::normalize(input_vector);
		} 

		/// ZERO OUT Y COMPONENT
		float Y_input_y_velocity = 0.0f;

		if (input.keyboard_state[KEY_SPACE])
		{
			want_to_jump = true;
			if (grounded)
			{
				float velocity = glm::length(adjusted_movement_vector);
				velocity *= 1.1f;
				adjusted_movement_vector *= velocity;

				logr::report("jumped\n");
				// if (input_velocity > 0.0000001f) //input_velocity = 1.0f * dt;
				// Y_input_y_velocity = pm_jump_acceleration * dt;
				j_time = 0.0f;

				grounded = false;
				jump_pressed_this_frame = true;
				want_to_jump = false;
			}
		}

		glm::vec3 movement_vector = glm::vec3(0.0f);
		// IF NO INPUT IS RECEIVED, CONTINUE IN THE SAME DIRECTION WITH SAME SPEED AS WE WERE ON.
		if (input_velocity < 0.00001f) 
		{
			// don't normalize: will yield nan or inf
			movement_vector = adjusted_movement_vector;
		}
		else
		{
			float acceleration = 0.0f;

			// POSSIBLY USE DIFFERENT ACCELERATION VALUES FOR WHEN WE ARE IN THE AIR.
			if (grounded)
			{
				acceleration = pm_ground_acceleration;
			}
			else
			{
				acceleration = pm_air_acceleration;
			}
			movement_vector = accelerate(adjusted_movement_vector, input_vector, input_velocity, acceleration, dt);
		}

		movement_vector.y = Y_old_y;
		movement_vector.y += Y_input_y_velocity;

		//Y
		if (!grounded)
		{
			j_time += dt;
			float x = j_time;
			float c = 0.0f;
			float b = pm_jump_acceleration;
			float a = -g_player_gravity;
			movement_vector.y = a * x * x + b * x + c; 
			// movement_vector.y -= g_player_gravity * dt;
			logr::report("movement_vector: {}\n", movement_vector);
		}
		
		// try to move (collide with ground plane etc etc etc)
		position = glm::vec3(old_position.x, 0.0f, old_position.z) + movement_vector;

		// // clip the movement vector.
		if (position.y < 0.0f)
		{
			end = std::chrono::system_clock::now();
			logr::report("jump duration: {}\n", static_cast<std::chrono::duration<double>>(end - start).count());
			grounded = true;
			position.y = 0.0f;
			movement_vector.y = 0.0f;
			j_time = 0.0f;
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
		Camera new_camera = camera;
	    glm::vec3 world_up(0.0f,1.0f, 0.0f);

	    float adjusted_x_offset = static_cast<float>(input.mouse_delta_x) * g_mouse_sensitivity;
	    float adjusted_y_offset = static_cast<float>(input.mouse_delta_y) * g_mouse_sensitivity;

	    new_camera.yaw   += adjusted_x_offset;
	    new_camera.pitch += adjusted_y_offset;

	    // make sure that when pitch is out of bounds, screen doesn't get flipped
	    if (should_constrain_pitch)
	    {
	        if (new_camera.pitch > 89.0f)  new_camera.pitch = 89.0f;
	        if (new_camera.pitch < -89.0f) new_camera.pitch = -89.0f;
	    }

	    // update front, right and up Vectors using the updated euler angles
	    glm::vec3 front{};
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

	Camera update_camera_entity(const Input& input, const Camera old_camera, const float dt)
	{
		Camera camera = old_camera;
		auto [position, movement_vector] = update_position_with_input(input, old_camera.position, old_camera.movement_vector, old_camera.front, old_camera.right, dt); 
		camera.position = position;
		camera.movement_vector = movement_vector;

		if (input.mouse_delta_x || input.mouse_delta_x)
			return update_camera_view_with_input(input, camera, dt);

		return camera;
	}

	void update_player_entity(const Input& input)
	{
		auto& player = g_player_entity;
	}

	void evaluate_flying_units(Entity_Manager& entity_manager, const float dt)
	{
		auto& player = g_player_entity;
		auto& dodecahedrons = by_type(entity_manager, Entity_Type::Cube);

    	auto sum_lambda = [](glm::vec3 sum, Entity& rhs) {
                         return (sum + rhs.position);
                     	};
        glm::vec3 sum = std::accumulate(dodecahedrons.begin(), dodecahedrons.end(), glm::vec3(0.0f), sum_lambda);

        //@Incomplete(Sjors): div 0?
        glm::vec3 average_position = sum *  (1.0f / static_cast<float>(dodecahedrons.size()));
        glm::vec3 player_position = player.position + glm::vec3(0.0f,10.0f,0.0f);
        glm::vec3 center_to_player_direction = glm::normalize(player_position - average_position);
		
		struct Neighbour_Info
		{
			glm::vec3 direction;
			float distance;
		};

		//@Memory(Sjors)
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

			entity.position = entity.position + (direction_vector * g_dodecahedron_velocity * dt);	
			entity.movement_vector = direction_vector;
		}

	}

	void evaluate_shot(Entity_Manager& entity_manager, Camera camera)
	{
		timed_function("evaluate_shot");

		std::vector<Entity*>& entities = by_type_ptr(entity_manager, Entity_Type::Cube);
		for (Entity* entity_ptr: entities)
		{
  			if (!ray_intersects_sphere(camera.position, camera.front, entity_ptr->position, 20.0f)) continue;
			// we hit.
			schedule_for_destruction(entity_manager, entity_ptr);
		}

	}



}

// I can't stress enough that we should absolutely NOT use dt.
// update and render world
void game_simulate(Game_State& game_state, const double dt, const Input& input, Particle_Cache& particle_cache, Entity_Manager& entity_manager)
{
	float clamped_dt = static_cast<float>(dt);	
	if (clamped_dt < FRAMETIME_1000_FPS)
	{
		clamped_dt = FRAMETIME_1000_FPS;	
	}

	if (clamped_dt > FRAMETIME_10_FPS)
	{
		clamped_dt = FRAMETIME_10_FPS;  	
	}

	// if (vsync) clamped_dt = FRAMETIME_144_FPS;
 
 	// logr::report("dt: {}\n", dt);

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
				game_state.camera = update_camera_entity(input, game_state.camera, clamped_dt);
				g_player_entity.position = game_state.camera.position;
			}

			// // update dodecahedrons
			// {
			// 	evaluate_flying_units(entity_manager, dt);
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
