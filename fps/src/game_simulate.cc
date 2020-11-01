#include "game.h"
#include "camera.h"
#include "input.h"
#include "log.h"
#include "entity_system.h"

// these are the GLFW key presses.
constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;

namespace
{
	//@FIXME(Sjors): to be replaced by a type_player entity.
	// uh, is player a camera?
	struct Player
	{
		uint32_t ID;
		glm::vec3 position;
	};

	// player entity fields.
   	Player g_player{0, glm::vec3{0.0f,0.0f,3.0f}};

    // ---------------
	// cvars
	// ---------------
    // input
    float g_mouse_sensitivity = 0.05f;
   
    // player movement
    float g_player_movespeed = 0.2f;
    float g_player_acceleration = 0.2f;
    float g_player_max_movespeed = 1.0f;

    //@FIXME: this depends on several factors.
    // Eventually, I want to re_split this again (camera vs player)
	glm::vec3 update_position_with_input(const Input& input, const glm::vec3 old_position, const glm::vec3 front, const glm::vec3 right)
	{
		glm::vec3 position = old_position;

		if (input.keyboard_state[KEY_W])
		{
	    	logr::report("[Simulate] pressed W.\n");

	    	position = position + (front * g_player_movespeed);

		}
		if (input.keyboard_state[KEY_S])
		{
	    	logr::report("[Simulate] pressed S.\n");

            position = position - (front * g_player_movespeed);

		}
		if (input.keyboard_state[KEY_A])
		{
	    	logr::report("[Simulate] pressed A.\n");

	   		position = position - (right * g_player_movespeed);

		}
		if (input.keyboard_state[KEY_D])
		{
	    	logr::report("[Simulate] pressed D.\n");

            position = position + (right * g_player_movespeed);
		}

		return position;

	}

	// processes input received from a mouse input system.
	// assumes world up direction is positive y.
	// @dependencies: 
	// mouse_sensitivity
	Camera update_camera_view_with_input(const Input& input, const Camera camera, const bool should_constrain_pitch = true)
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

	Camera update_camera_entity(const Input& input, const Camera old_camera)
	{
		Camera camera = old_camera;
		camera.position = update_position_with_input(input, old_camera.position, old_camera.front, old_camera.right);
		if (input.mouse_delta_x || input.mouse_delta_x)
			return update_camera_view_with_input(input, camera);

		return camera;
	}

}


// update and render world
void game_simulate(const float dt, Game_State& game_state,const Input& input, Particle_Cache& particle_cache)
{
	//@TODO(Sjors): set bounds for min / max frame time.
	// if (dt < 0.0001)
	// if (dt > 0.1)

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
			game_state.camera = update_camera_entity(input, game_state.camera);
			g_player.position = game_state.camera.position;
		}
	}

	// BEGIN_SIMULATION




	// execute brains (update and render entities());


	// update and render particle systems();


}
