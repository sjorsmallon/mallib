#include "game.h"
#include "camera.h"
#include "input.h"
#include "log.h"

// these are the GLFW key presses.
constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;


namespace
{
	Camera g_player_camera = default_camera();
    float g_mouse_sensitivity = 0.05f;
    glm::vec3 g_player_movement_vector{};
    float g_player_movespeed = 0.2f;
    float g_player_acceleration = 0.2f;
    float g_player_max_movespeed = 1.0f;

	void update_player_position_with_input(const Input& input)
	{
	    // logr::report("[Renderer] updating player position.\n");

		if (input.keyboard_state[KEY_W])
		{
	    	logr::report("[Simulate] pressed W.\n");

	    	g_player_camera.position = g_player_camera.position + (g_player_camera.front * g_player_movespeed);

		}
		if (input.keyboard_state[KEY_S])
		{
	    	logr::report("[Simulate] pressed S.\n");

            g_player_camera.position = g_player_camera.position - (g_player_camera.front * g_player_movespeed);

		}
		if (input.keyboard_state[KEY_A])
		{
	    	logr::report("[Simulate] pressed A.\n");

	   		g_player_camera.position = g_player_camera.position + (g_player_camera.right * g_player_movespeed);

		}
		if (input.keyboard_state[KEY_D])
		{
	    	logr::report("[Simulate] pressed D.\n");

            g_player_camera.position = g_player_camera.position + (g_player_camera.right * g_player_movespeed);
		}

	    // switch (key_pressed)
	    // {
	    //     case KEY_W:
	    //     {
	    //         g_player_camera.position = g_player_camera.position + (g_player_camera.front * g_player_movespeed);
	    //         break;
	    //     }
	    //     case KEY_S:
	    //     {
	    //         g_player_camera.position = g_player_camera.position - (g_player_camera.front * g_player_movespeed);

	    //         break;
	    //     }
	    //     case KEY_A:
	    //     {
	    //         g_player_camera.position = g_player_camera.position - (g_player_camera.right * g_player_movespeed);

	    //         break;
	    //     }
	    //     case KEY_D:
	    //     {
	    //         g_player_camera.position = g_player_camera.position + (g_player_camera.right * g_player_movespeed);
	    //         break;
	    //     }
	    //     default:
	    //     {
	    //         logr::report_warning("[Renderer] WARNING: Key pressed that was not recognized. value: {}", key_pressed);
	    //         break;
	    //     }
	    // }
	}

	// // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	// assumes world up direction is positive y.
	// @dependencies: 
	// mouse_sensitivity
	// player_camera
	void update_player_camera_with_mouse_input(const Input& input, const bool should_constrain_pitch = true)
	{

	    // logr::report("[Renderer] updating player camera.\n");
	    
	    glm::vec3 world_up(0.0f,1.0f, 0.0f);

	    float adjusted_x_offset = input.mouse_delta_x * g_mouse_sensitivity;
	    float adjusted_y_offset = input.mouse_delta_y * g_mouse_sensitivity;

	    g_player_camera.yaw   += adjusted_x_offset;
	    g_player_camera.pitch += adjusted_y_offset;

	    // make sure that when pitch is out of bounds, screen doesn't get flipped
	    if (should_constrain_pitch)
	    {
	        if (g_player_camera.pitch > 89.0f)  g_player_camera.pitch = 89.0f;
	        if (g_player_camera.pitch < -89.0f) g_player_camera.pitch = -89.0f;
	    }

	    // update front, right and up Vectors using the updated euler angles
	    glm::vec3 front;
	    front.x = cos(glm::radians(g_player_camera.yaw)) * cos(glm::radians(g_player_camera.pitch));
	    front.y = sin(glm::radians(g_player_camera.pitch));
	    front.z = sin(glm::radians(g_player_camera.yaw)) * cos(glm::radians(g_player_camera.pitch));
	    g_player_camera.front = glm::normalize(front);
	    // also re-calculate the right and up vector
	    g_player_camera.right = glm::normalize(glm::cross(g_player_camera.front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	    g_player_camera.up    = glm::normalize(glm::cross(g_player_camera.right, g_player_camera.front));
	}

	void update_player(const Input& input)
	{
		update_player_position_with_input(input);
		if (input.mouse_delta_x || input.mouse_delta_x) update_player_camera_with_mouse_input(input);
	}

}




// update and render world
void simulate(const float dt, Game_State& game_state,const Input& input, Particle_Cache& particle_cache)
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
		// update player position && update camera
		update_player(input);
	}

	// BEGIN_SIMULATION




	// execute brains (update and render entities());


	// update and render particle systems();


}
