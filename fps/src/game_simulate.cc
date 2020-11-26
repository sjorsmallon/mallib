#include "game.h"

#include <glm/gtx/string_cast.hpp>
#include <algorithm> // for clamp, kind of wasteful!

#include "camera.h"
#include "input.h"
#include "log.h"
#include "entity_manager.h"
#include "sound_system.h"
#include "collision_system.h"

// these are the GLFW key presses.
constexpr const int KEY_SPACE = 32;
constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;
constexpr const int KEY_P = 80;
constexpr const int KEY_O = 79;
constexpr const int KEY_I = 73;

constexpr const float FRAMETIME_120_FPS = 0.00833333333f;
constexpr const float FRAMETIME_IN_S = FRAMETIME_120_FPS;

constexpr const float FRAMETIME_1000_FPS = 0.001f;
constexpr const float FRAMETIME_10_FPS = 0.1f;


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
    float g_mouse_sensitivity = 0.5f;
    float g_camera_velocity = 0.2f;

    float g_dodecahedron_velocity = 1.25f;

 	float g_player_gravity = 0.3335f;
    float g_player_friction = 0.1f;

    float g_player_max_velocity = 3.75f;

    float g_player_ground_movespeed = 0.043125f;
    float g_player_ground_acceleration = 0.08625f;
    float g_player_ground_deceleration= 0.06187f;
    float g_player_jump_velocity = 0.0665f;

    float g_player_air_acceleration = 0.033f;
    float g_player_air_deceleration = 0.033f;
    float g_player_air_control = 0.01f;
    float g_player_side_strafe_acceleration = 1.6667f;
    float g_player_side_strafe_speed = 0.0033f;

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

		const glm::vec3 world_up(0.0f, 1.0f, 0.0f);
		const glm::vec3 front_without_height = glm::vec3(front.x, 0.0f, front.z);

		glm::vec3 input_vector = glm::vec3(0.0f);
		if (input.keyboard_state[KEY_W]) input_vector += front_without_height;
		if (input.keyboard_state[KEY_S]) input_vector -= front_without_height;
		if (input.keyboard_state[KEY_A]) input_vector -= right;
		if (input.keyboard_state[KEY_D]) input_vector += right;
		if (input.keyboard_state[KEY_SPACE])
		{
			if (grounded)
			{
				input_vector.y += g_player_jump_velocity; 
				grounded = false;
				jump_pressed_this_frame = true;
			}
		}

		// ground control (to major tom)
		glm::vec3 input_ground_vector = glm::vec3(input_vector.x, 0.0f, input_vector.z);
    	float     input_ground_velocity = g_player_ground_movespeed * dt_factor;
    	input_ground_vector = input_ground_vector * input_ground_velocity;

		glm::vec3 old_ground_vector =   glm::vec3(old_movement_vector.x, 0.0f, old_movement_vector.z);
		float old_ground_vector_velocity = glm::length(old_ground_vector);

    	glm::vec3 ground_vector = old_ground_vector + input_ground_vector;
		float ground_velocity = glm::length(ground_vector);
    	

    	glm::vec3 movement_vector(0.0f);
    	movement_vector.x = ground_vector.x;
    	movement_vector.y = 0.0f;
    	movement_vector.z = ground_vector.z;

		// Do not apply friction if the player is queueing up the next jump
		if(!jump_pressed_this_frame)
		{
			float drop = 0.0f;

			//@FIXME(Sjors): Only if the player is on the ground then apply friction
			// if (grounded)
			// {
				float control = (ground_velocity < g_player_ground_acceleration) ? g_player_ground_deceleration : ground_velocity;
				drop = control * g_player_friction * dt_factor;
			// } 

			float drop_adjusted_velocity = ground_velocity - drop;
			if(drop_adjusted_velocity < 0.0f) drop_adjusted_velocity = 0.0f;
			
			if(ground_velocity > 0.0f) drop_adjusted_velocity /= ground_velocity;
			movement_vector = ground_vector * drop_adjusted_velocity;
		}
		
		if (old_movement_vector.y != 0.0f)  movement_vector.y = old_movement_vector.y;
		if (input_vector.y > 0.0f) movement_vector.y = input_vector.y;

		//@FIXME(Sjors): do air movement here.

		// @FIXME(Sjors): only apply gravity when IN the air.
		if (old_position.y > 0.0f) movement_vector.y -= g_player_gravity * 0.008f * dt_factor;

		glm::vec3 position = old_position + movement_vector;

		// clip the movement vector.
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


	void update_flying_units(Entity_Manager& entity_manager, const float dt_factor)
	{
		auto& player = g_player_entity;

		// separation: steer to avoid crowding local flockmates
		// alignment: steer towards the average heading of local flockmates
		// cohesion: steer to move towards the average position (center of mass) of local flockmates
		// wat we nu gaan doen, kost heel veel tijd.
		auto dodecahedrons = by_type(entity_manager, Entity_Type::Cube);

		for (auto* entity_ptr: dodecahedrons)
		{
			glm::vec3 direction_vector = glm::normalize(player.position - entity_ptr->position);
			entity_ptr->position = entity_ptr->position + (direction_vector * g_dodecahedron_velocity * dt_factor);
		}



		// separation: steer to avoid crowding local flockmates
		// alignment: steer towards the average heading of local flockmates
		// cohesion: steer to move towards the average position (center of mass) of local flockmates

	}


}

// I can't stress enough that we should absolutely NOT use dt.
// update and render world
void game_simulate(const double dt, Game_State& game_state, const Input& input, Particle_Cache& particle_cache, Entity_Manager& entity_manager)
{
	float clamped_dt = dt;	
	if (clamped_dt < 0.00001) clamped_dt = FRAMETIME_1000_FPS; // 1000 fps: upper bound
	if (clamped_dt > FRAMETIME_10_FPS) clamped_dt = FRAMETIME_10_FPS;  // 10 fps: lower bound
 
 	const float dt_factor =  clamped_dt / FRAMETIME_IN_S;

	// process higher level input
	{
		if (input.keyboard_state[KEY_P]) game_state.game_mode = GM_GAME;
		if (input.keyboard_state[KEY_O]) game_state.game_mode = GM_EDITOR;
		if (input.keyboard_state[KEY_I]) game_state.paused = 1 - game_state.paused; // toggle;
	}

	// are we paused?
	if (game_state.paused)
	{

	}
	else
	{

		// if game_mode = player_cam:
		{
			//@Note(Sjors): it is imperative that the player gets updated first, since that 
			// is the bottleneck we have to be dealing with in the next frames.

			// update player and camera.
			{
				update_player_entity(input);
				game_state.camera = update_camera_entity(input, game_state.camera, dt_factor);
				g_player_entity.position = game_state.camera.position;
			}

			// update dodecahedrons
			{
				update_flying_units(entity_manager, dt_factor);
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
				if (input.mouse_left) play_sound("plop.wav");
			}
		}


	}


}
