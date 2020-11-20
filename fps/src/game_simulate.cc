#include "game.h"
#include "camera.h"
#include "input.h"
#include "log.h"
#include "entity_system.h"
#include <glm/gtx/string_cast.hpp>


// these are the GLFW key presses.
constexpr const int KEY_SPACE = 32;
constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;

constexpr const float FRAMETIME_120_HZ_IN_S = 0.00833333333f;

namespace
{
	//@FIXME(Sjors): to be replaced by a type_player entity.
	// uh, is player a camera?
	struct Player
	{
		uint32_t ID;
		glm::vec3 position;
		glm::vec3 movement_vector;
		glm::vec3 velocity;
	};

	// player entity fields.
   	Player g_player{0, glm::vec3{0.0f,0.0f,3.0f}};

    // ---------------
	// cvars
	// ---------------
    float g_mouse_sensitivity = 0.05f;

    // player movement (120 hz baseline tickrate).
    float g_player_movespeed = 0.2f;
    float g_player_acceleration = 2.0f;
    float g_player_max_velocity = 10.0f;
    float g_camera_movespeed = 0.2f;

    // @dependencies:
    // g_camera_movespeed
	glm::vec3 update_flying_camera_with_input(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 front,
		const glm::vec3 right,
		const glm::vec3 up,
		const float dt)
	{
		float dt_factor = dt / FRAMETIME_120_HZ_IN_S;
		glm::vec3 position = old_position;

		if (input.keyboard_state[KEY_W])
		{
	    	position = position + (front * g_camera_movespeed * dt_factor);
		}
		if (input.keyboard_state[KEY_S])
		{
            position = position - (front * g_camera_movespeed * dt_factor);
		}
		if (input.keyboard_state[KEY_A])
		{
	   		position = position - (right * g_camera_movespeed * dt_factor);
		}
		if (input.keyboard_state[KEY_D])
		{
            position = position + (right * g_camera_movespeed * dt_factor);
		}
		if (input.keyboard_state[KEY_SPACE])
		{
			position = position + (up * g_camera_movespeed * dt_factor);
		}

		return position;
	}

    //@Dependencies:
    // g_player_movespeed
    // g_player_acceleration
    // world_up
    // FRAMETIME_120_HZ
	glm::vec3 update_position_with_input(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 old_movement_vector,
		const glm::vec3 front,
		const glm::vec3 right,
		const float dt)
	{
		// use the FACTOR here.
		const float dt_factor = dt / FRAMETIME_120_HZ_IN_S;
		const glm::vec3 front_without_height = glm::vec3(front.x, 0.f, front.z);
		const glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
		const float old_velocity = glm::length(old_movement_vector);


		glm::vec3 input_movement_vector = glm::vec3(0.0f);
		if (input.keyboard_state[KEY_W])
		{
	    	input_movement_vector += front;
		}
		if (input.keyboard_state[KEY_S])
		{
	    	input_movement_vector -= front;
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
			input_movement_vector += world_up; 
			if (old_velocity > 0.02f) input_movement_vector += 0.2f* old_movement_vector;
		}

		float input_vector_velocity = g_player_acceleration * dt_factor;
    	input_movement_vector = input_movement_vector * input_vector_velocity;

    	glm::vec3 movement_vector = old_movement_vector + input_movement_vector;
    	float velocity = glm::length(movement_vector);
    	// clip velocity if necessary
    	velocity = (velocity > g_player_max_velocity) ? g_player_max_velocity : velocity;
    	glm::vec3 position = old_position + velocity * movement_vector;

		return position;
	}

	// processes input received from a mouse input system.
	// assumes world up direction is positive y.
	// @Note(Sjors): this does not use dt!
	// @dependencies: 
	// g_mouse_sensitivity
	Camera update_camera_view_with_input(const Input& input, const Camera camera, const float dt, const bool should_constrain_pitch = true)
	{
		Camera new_camera = camera;
	    // logr::report("[Renderer] updating player camera.\n");
	    glm::vec3 world_up(0.0f,1.0f, 0.0f);

	    float adjusted_x_offset = input.mouse_delta_x * g_mouse_sensitivity;
	    float adjusted_y_offset = input.mouse_delta_y * g_mouse_sensitivity;

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
	    new_camera.right = glm::normalize(glm::cross(new_camera.front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
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
		camera.position = update_position_with_input(input, old_camera.position, old_camera.movement_vector, old_camera.front, old_camera.right, dt);
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



	if (game_state.game_mode == GM_EDITOR)
	{
		if (input.mouse_right)
		{
			game_state.camera = update_camera_entity(input, game_state.camera, clamped_dt);
		}
		else
		{

		}

	}


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
			game_state.camera = update_camera_entity(input, game_state.camera, clamped_dt);
			g_player.position = game_state.camera.position;
		}
	}

	// BEGIN_SIMULATION




	// execute brains (update and render entities());


	// update and render particle systems();


}
