#include "game.h"
#include "camera.h"
#include "input.h"
#include "log.h"
#include "entity_system.h"
#include <glm/gtx/string_cast.hpp>
#include <algorithm> // for clamp


// these are the GLFW key presses.
constexpr const int KEY_SPACE = 32;
constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;
constexpr const int KEY_P = 80;
constexpr const int KEY_O = 79;


constexpr const float FRAMETIME_120_HZ_IN_S = 0.00833333333f;

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
   	Player g_player{0, glm::vec3{0.0f,0.0f,3.0f}, glm::vec3{0.0f,0.0f,0.0f}};

    // ---------------
	// cvars
	// ---------------
    float g_mouse_sensitivity = 0.5f;

    // player movement (120 hz baseline tickrate).
    float g_player_acceleration = 0.05f;
    float g_player_max_velocity = 0.5f;
    float g_camera_velocity = 0.2f;
    const glm::vec3 g_world_gravity = glm::vec3(0.f, 0.5f,0.0f);

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

		if (input.keyboard_state[KEY_W])
		{
	    	position = position + (front * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_S])
		{
            position = position - (front * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_A])
		{
	   		position = position - (right * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_D])
		{
            position = position + (right * g_camera_velocity * dt_factor);
		}
		if (input.keyboard_state[KEY_SPACE])
		{
			position = position + (up * g_camera_velocity * dt_factor);
		}

		return position;
	}

    //@Dependencies:
    // g_player_movespeed
    // g_player_acceleration
    // world_up
    // FRAMETIME_120_HZ
	std::tuple<glm::vec3, glm::vec3> update_position_with_input(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 old_movement_vector,
		const glm::vec3 front,
		const glm::vec3 right,
		const float dt_factor)
	{
		// use the FACTOR here.
		const glm::vec3 front_without_height = glm::vec3(front.x, 0.f, front.z);
		const glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
		const float old_velocity = glm::length(old_movement_vector);

		glm::vec3 input_movement_vector = glm::vec3(0.0f);
		if (input.keyboard_state[KEY_W])
		{
			input_movement_vector += front_without_height;
		}
		if (input.keyboard_state[KEY_S])
		{
			input_movement_vector -= front_without_height;
		}
		if (input.keyboard_state[KEY_A])
		{
			input_movement_vector -= right;
		}
		if (input.keyboard_state[KEY_D])
		{
			input_movement_vector += right;
		}
		if (input.keyboard_state[KEY_SPACE])
		{
			//@FIXME(Sjors): this is not a very robust system.
			// if we are on the ground, we can jump.
			// how do we verify we are on the ground? we can jump off platforms at arbitrary height,
			// so checking y value against 0 does not work. 
			// checking the movement vector also does not work: if v_y reaches 0 at the 
			// apex of the jump, it would be possible to jump again mid air.
			// both of these imply some sort of state to decide whether we are grounded or not.
			// for now, we are "grounded" if y is equal to 0.0f. 
			bool grounded = (fabs(old_position.y) < 0.01f);
			if (grounded)
			{
				float up_velocity = 10.0f;
				input_movement_vector += world_up * up_velocity; 
				//@TODO(Sjors): imbue jump with a bit of forward momentum.
				// if (old_velocity > 0.02f) input_movement_vector += 0.02f* old_movement_vector;	
			}
		}

		// if affected by gravity: (only if the vector has positive or negative y):
		bool grounded = (fabs(old_position.y) < 0.1f);
		if (!grounded) input_movement_vector -= g_world_gravity * dt_factor;

		float input_vector_velocity = g_player_acceleration * dt_factor;
    	input_movement_vector = input_movement_vector * input_vector_velocity;

    	glm::vec3 movement_vector = old_movement_vector + input_movement_vector;

    	//3D:
    	// float velocity = glm::length(movement_vector);
  		// clamp velocity if necessary 
  		// velocity = std::clamp(velocity, 0.0f, g_player_max_velocity);
    	// since velocity can be clipped, recalculate the movement vector.
    	// if (velocity > 0.01f) movement_vector = glm::normalize(movement_vector) * velocity;
    	// glm::vec3 position = old_position + velocity * movement_vector;

    	float movement_vector_y = movement_vector.y; 
    	glm::vec2 xz(movement_vector.x, movement_vector.z);
    	float xz_velocity = glm::length(xz);
    	xz_velocity = std::clamp(xz_velocity, 0.0f, g_player_max_velocity);

    	// think about grounded ness.
    	// if (grounded) xz_velocity = std::clamp(xz_velocity, 0.0f, g_player_max_velocity);

    	if (xz_velocity > 0.01f) xz = glm::normalize(xz) * xz_velocity;
    	movement_vector.x = xz.x;
    	movement_vector.z = xz.y; 
    	movement_vector.y = movement_vector_y;


    	glm::vec3 position = old_position + glm::vec3(xz.x * xz_velocity, movement_vector_y, xz.y * xz_velocity);



    	// are we beneath the ground? move to ground & flatten gravity impact.
    	if (position.y < 0.0f)
		{
			position.y = 0.0f;
    		movement_vector.y = 0.0f;	
		}

    	auto result = std::make_tuple(position, movement_vector);
		return result;
	}

	// processes input received from a mouse input system.
	// assumes world up direction is positive y.
	// @Note(Sjors): this does not use dt!
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

	void update_player_entity(const Input& input)
	{
		auto& player = g_player;
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
}


// update and render world
void game_simulate(const double dt, Game_State& game_state,const Input& input, Particle_Cache& particle_cache)
{
	//@TODO(Sjors): set bounds for min / max frame time.
	float clamped_dt = dt;
	if (clamped_dt < 0.001f)
	{
			// logr::report("clamped dt.\n");
			clamped_dt = 0.001f;
	}	
	if (clamped_dt > 0.1f) clamped_dt = 0.1f;


	if (input.keyboard_state[KEY_P]) game_state.game_mode = GM_GAME;
	if (input.keyboard_state[KEY_O]) game_state.game_mode = GM_EDITOR;


	float dt_factor = clamped_dt / FRAMETIME_120_HZ_IN_S;



	// are we paused?
	if (game_state.paused)
	{

	}
	else
	{
		// update entities
		// for (entity_type : entity_types)
		// update camera position.
		// if game_mode = player_cam:
		{
			update_player_entity(input);
			game_state.camera = update_camera_entity(input, game_state.camera, dt_factor);
			g_player.position = game_state.camera.position;
		}
	}

	// BEGIN_SIMULATION
	// execute brains (update and render entities());


	// update and render particle systems();


}
