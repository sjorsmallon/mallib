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
    float pm_ground_acceleration = 10.f;
    float pm_air_acceleration = 1.0f;
    // quake constants
    float pm_friction = 6.f;
    float pm_stopspeed = 100.0f;
    float pm_maxvelocity = 320.0f;


   	float g_gravity = 800.0f;
    float pm_jumpvelocity = 270.0f;

   	float input_axial_extreme = 127.0f; // -127, 127 for forward and backward (for controller support)

    float g_mouse_sensitivity = 0.08f;
    float g_camera_velocity = 0.2f;



 	float CLIP_OVERCLIP = 1.001f;
    
    

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
		if (grounded) old_movement_vector.y = 0.0f;

		float velocity = glm::length(old_movement_vector);

		//@Cleanup(Sjors): fix the velocity flooring threshold.
		const float velocity_treshold = 0.1f;
		if (velocity < velocity_treshold)
		{
			logr::report("velocity < velocity_treshold\n");
			return glm::vec3(0.0f);
		}

		float velocity_drop = 0.0f;

		if (grounded && !jump_pressed_this_frame)
		{
			float control = velocity < pm_stopspeed ? pm_stopspeed : velocity;
			velocity_drop += control * pm_friction * dt;
		}

		// adjust the velocity with the induced velocity drop. 
		float adjusted_velocity = velocity - velocity_drop;

		// cannot move in the negatives.
		if(adjusted_velocity < 0.0f) adjusted_velocity = 0.0f;

		// normalize the velocity based on the ground velocity.
		if(adjusted_velocity > 0.0f) adjusted_velocity /= velocity;

		//@IC(Sjors): we don't need to the movement vector below since we do that above (normalize the new_velocity by velocity)
		return old_movement_vector * adjusted_velocity;
	}

	// since input can be provided -127 -> +127, we scale the movement vector based on the input delivered.
    [[nodiscard]]
    float calculate_input_scale(const float forward_move,const float right_move, const float up_move, const float max_velocity)
    {
    	//@Cleanup(Sjors): get rid of these casts.
    	int max = abs(static_cast<int>(forward_move));
    	if (abs(static_cast<int>(right_move)) > max) max = abs(static_cast<int>(right_move));

    	if (abs(static_cast<int>(up_move)) > max) max = abs(static_cast<int>(up_move));

    	if (!max) return 0.f;

    	float total = sqrt(forward_move * forward_move + right_move * right_move + up_move * up_move);
    	float scale = pm_maxvelocity * static_cast<float>(max) / (input_axial_extreme * total);
    	return scale;
    }

    [[nodiscard]]
    glm::vec3 clip_velocity(glm::vec3 in, glm::vec3 normal, const float overbounce)
    {
    	float backoff = glm::dot(in, normal);

    	if (backoff < 0.0f)
    	{
    		backoff *= overbounce;
    	}
    	else
    	{
    		backoff /= overbounce;
    	}

    	glm::vec3 change = normal * backoff;
    	glm::vec3 result = in - change;

    	return result;
    }

	//@TODO(Sjors) NaN checks etc.
	[[nodiscard]]
	glm::vec3 accelerate(glm::vec3 adjusted_movement_vector, glm::vec3 wish_direction, float wish_velocity, float acceleration, float dt)
	{
		float current_speed_in_wish_dir = glm::dot(adjusted_movement_vector, wish_direction);
		float add_speed = wish_velocity - current_speed_in_wish_dir;

		if (add_speed < 0.0f) return adjusted_movement_vector;

		float acceleration_speed = acceleration * dt * wish_velocity;
		if (acceleration_speed > add_speed) acceleration_speed = add_speed;

		glm::vec3 result = adjusted_movement_vector + acceleration_speed * wish_direction;

		// //@FIXME(Sjors): NaN check.
		// if ((current_speed_in_wish_dir != current_speed_in_wish_dir) && adjusted_velocity > 0.0f)
		// {
		// 	logr::report_warning("opposite directions pressed..\n");
		// 	return glm::vec3(0.0f);
		// }

		// logr::report("result: {}\n", result);
		// if (adjusted_velocity < 0.0000001f)
		// {
		// 	logr::report_warning("adjusted_vector velocity too small, returning acceleration vector\n");
		// 	return acceleration_speed * wish_direction;
		// }

		return result;
	}	

	// if we succeeded and did not clip it, ....
	// glm::vec3 slide_move(const glm::vec3 old_position, const glm::vec3 movement_vector, const bool player_is_airborne, const float dt)
	// {
	// 	if (player_is_airborne)
	// 	{
	// 		glm::vec3 new_movement_vector = movement_vector;
	// 		new_movement_vector.y -= g_gravity * dt;
	// 	}

	// 	glm::vec3 position = old_position + (new_movement_vector * dt);
	// 	return position;
	// }

	// ah, I guess this is to check for incremental changes in height.
	//@Incomplete: check against geometry?
	// dependencies:
	// g_gravity
	[[nodiscard]]
	std::tuple<glm::vec3, glm::vec3> step_slide_move(const glm::vec3 old_position, const glm::vec3 movement_vector, const bool player_is_airborne, const float dt)
	{
		glm::vec3 new_movement_vector = movement_vector;
		if (player_is_airborne)
		{
			logr::report("player_is_airborne: {}\n", player_is_airborne);
			new_movement_vector.y -= g_gravity * dt;
			logr::report("new_movement_vector.y: {}\n", new_movement_vector.y);
		}
		glm::vec3 position = old_position + (new_movement_vector * dt);
		return std::make_tuple(position, new_movement_vector);
	}

	[[nodiscard]]
	bool check_jump(const Input& input)
	{
		// conditions that deal with jumping.
		// if held, wait for release. Otherwise, if on the ground & ...
		return input.keyboard_state[KEY_SPACE];
	}

	std::tuple<glm::vec3, glm::vec3> air_move(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 old_movement_vector,
		const glm::vec3 front,
		const glm::vec3 right,
		const float dt)
	{
		logr::report_warning("[air_move]\n");
		bool grounded = false;
		bool jump_pressed_this_frame = false;
		
		glm::vec3 adjusted_movement_vector = apply_friction(old_movement_vector, grounded, jump_pressed_this_frame, dt);
		float forward_input = input_axial_extreme * input.keyboard_state[KEY_W] - input_axial_extreme * input.keyboard_state[KEY_S];
		float right_input   = input_axial_extreme * input.keyboard_state[KEY_D] - input_axial_extreme * input.keyboard_state[KEY_A];
		float up_input      = input_axial_extreme;
		float scale = calculate_input_scale(forward_input, right_input, up_input, pm_maxvelocity);

		glm::vec3 plane_front = glm::vec3(front.x, 0.0f, front.z);
		glm::vec3 plane_right = glm::vec3(right.x, 0.0f, right.z);
		plane_front = clip_velocity(plane_front, glm::vec3(0.0f,1.0f,0.0f), CLIP_OVERCLIP);
		plane_right = clip_velocity(plane_right, glm::vec3(0.0f,1.0f,0.0f), CLIP_OVERCLIP);
		plane_front = glm::normalize(plane_front);
		plane_right = glm::normalize(plane_right);

		glm::vec3 wish_direction = plane_front * forward_input + plane_right * right_input;
		float wish_velocity = 0.0f;
		//@Incomplete(Sjors): is there a difference between generating new input & just finalizing?
		bool received_input = (input.keyboard_state[KEY_W] ||
							   input.keyboard_state[KEY_S] ||
							   input.keyboard_state[KEY_A] ||
							   input.keyboard_state[KEY_D]);

		// normalize input vector, but keep velocity! this HAS to be normalized by dt.
		if (received_input)
		{
			wish_velocity  = scale * glm::length(wish_direction);
			wish_direction = glm::normalize(wish_direction);
		} 

		// not on ground!
		glm::vec3 movement_vector = accelerate(old_movement_vector, wish_direction, wish_velocity, pm_air_acceleration, dt);

		bool player_is_airborne = true;
	
		return step_slide_move(old_position, movement_vector, player_is_airborne, dt);
	}
    //@Dependencies:
    // g_player_acceleration
    // world_up
	std::tuple<glm::vec3, glm::vec3> walk_move(
		const Input& input,
		const glm::vec3 old_position,
		const glm::vec3 old_movement_vector,
		const glm::vec3 front,
		const glm::vec3 right,
		const float dt)
	{

		static bool grounded = true;
		bool jump_pressed_this_frame = check_jump(input);

		if (jump_pressed_this_frame && grounded)
		{
			grounded = false;
			glm::vec3 jump_movement_vector = old_movement_vector;
			jump_movement_vector.y = pm_jumpvelocity;
			return air_move(input, old_position, jump_movement_vector, front, right, dt);	
		}

		// otherwise, we wouldn't be in here!
		grounded = true;
		// apply friction
		glm::vec3 adjusted_movement_vector = apply_friction(old_movement_vector, grounded, jump_pressed_this_frame, dt);
		float forward_input = input_axial_extreme * input.keyboard_state[KEY_W] - input_axial_extreme * input.keyboard_state[KEY_S];
		float right_input   = input_axial_extreme * input.keyboard_state[KEY_D] - input_axial_extreme * input.keyboard_state[KEY_A];
		float up_input      = input_axial_extreme * (jump_pressed_this_frame && grounded);
		float scale = calculate_input_scale(forward_input, right_input, up_input, pm_maxvelocity);

		glm::vec3 plane_front = glm::vec3(front.x, 0.0f, front.z);
		glm::vec3 plane_right = glm::vec3(right.x, 0.0f, right.z);
		plane_front = clip_velocity(plane_front, glm::vec3(0.0f,1.0f,0.0f), CLIP_OVERCLIP);
		plane_right = clip_velocity(plane_right, glm::vec3(0.0f,1.0f,0.0f), CLIP_OVERCLIP);
		plane_front = glm::normalize(plane_front);
		plane_right = glm::normalize(plane_right);

		glm::vec3 wish_direction = plane_front * forward_input + plane_right * right_input;
		float wish_velocity = 0.0f;

		//@Incomplete(Sjors): is there a difference between generating new input & just finalizing?
		bool received_input = (input.keyboard_state[KEY_W] ||
							   input.keyboard_state[KEY_S] ||
							   input.keyboard_state[KEY_A] ||
							   input.keyboard_state[KEY_D]);

		// normalize input vector, but keep velocity! this HAS to be normalized by dt.
		if (received_input)
		{
			wish_velocity  = scale * glm::length(wish_direction);
			wish_direction = glm::normalize(wish_direction);
		} 
		// if we are ducking
		// if (player_state & PMF_DUCKED) wish_speed = pm_maxspeed * pm_duckscale (pm_duckscale = 0.5)
		
		glm::vec3 movement_vector = glm::vec3(0.0f);

		if (wish_velocity < 0.1f) 
		{
			//@Note(Sjors) don't normalize: will yield nan or inf
			movement_vector = adjusted_movement_vector;
			logr::report("no input!\n");
		}
		else
		{
			// Take into account whether we need air value or not.
			float acceleration = (grounded) ? pm_ground_acceleration : pm_air_acceleration;
			movement_vector = accelerate(adjusted_movement_vector, wish_direction, wish_velocity, acceleration, dt);
		}

		// slide along the ground plane.
		// clip the movement vector to not move into the ground.
		float velocity  = glm::length(movement_vector);
		movement_vector = clip_velocity(movement_vector, glm::vec3(0.0f,1.0f,0.0f), CLIP_OVERCLIP);

		//@FIXME(Sjors): formalize this threshold.
		float movement_treshold = 0.05f;

		//@TEMPORARY!!!!!! the normalize below causes NaN if movement_vector is zero.
		if (fabs(movement_vector.x) < movement_treshold && fabs(movement_vector.z) < movement_treshold)
		{
			return std::make_tuple(old_position, glm::vec3(0.0f, movement_vector.y, 0.0f));
		}

		// don't decrease velocity when going down a slope.
		movement_vector = glm::normalize(movement_vector);
		movement_vector = velocity * movement_vector;

		// skip checking the movement vector if the movement vector is too small.
		if (fabs(movement_vector.x) < movement_treshold && fabs(movement_vector.z) < movement_treshold)
		{
			return std::make_tuple(old_position, glm::vec3(0.0f, movement_vector.y, 0.0f));
		}

		bool player_is_airborne = false;

		return step_slide_move(old_position, movement_vector, player_is_airborne, dt);
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

		if (old_camera.position.y  > 0.0f)
		{
			auto [position, movement_vector] = air_move(input, old_camera.position, old_camera.movement_vector, old_camera.front, old_camera.right, dt);
			camera.position = position;
			camera.movement_vector = movement_vector;
		}
		else
		{
			auto [position, movement_vector] = walk_move(input, old_camera.position, old_camera.movement_vector, old_camera.front, old_camera.right, dt); 
			camera.position = position;
			camera.movement_vector = movement_vector;
		}
	
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
		glm::vec3 sum{};
		size_t entity_count = 0;
		for (auto&& entity: by_type(entity_manager, Entity_Type::Cube))
		{	
			sum += entity.position;
			entity_count += 1;
		}

		glm::vec3 average_position{};
		if (entity_count > 0) average_position = sum / static_cast<float>(entity_count);


		auto& player = g_player_entity;

		for (auto&& entity: by_type(entity_manager, Entity_Type::Cube))
		{

			glm::vec3 focus_direction      = glm::normalize(player.position - entity.position);
			glm::vec3 cohesion_direction   = glm::normalize(average_position - entity.position);
			glm::vec3 height_direction     = glm::vec3(0.0f,1.0f,0.0f);
			glm::vec3 cluster_center_to_player_direction = glm::normalize(player.position - average_position);

			bool enable_cohesion = true;
			bool enable_height = true;
			bool enable_separation = false;
			bool enable_focus = true;
			bool enable_previous_momentum = true;
			bool enable_center_to_player = true;

			glm::vec3 direction_vector{};
			if (enable_cohesion)	direction_vector += cohesion_direction * g_cohesion;
			if (enable_height)		direction_vector += height_direction;

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
			play_sound_at_position("finger_snap", entity.position.x, entity.position.y, entity.position.z);
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
		static bool done = false;
		if (!done) 
		{
			// BEFORE MOVING ANYTHING, check shot intersection, since we clicked at the position we _are_ in
			// and target that _are _ in a particular position
			if (input.mouse_left) evaluate_shot(entity_manager, game_state.camera);

			//@Note(Sjors): it is imperative that the player gets updated first, since that 
			// is the bottleneck we have to be dealing with in the next frames. (after, we can go wide).
		}
			// update player and camera.
			{

				game_state.camera = update_camera_entity(input, game_state.camera, clamped_dt);
				g_player_entity.position = game_state.camera.position;
			}

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
			}
			
		if (!done)
		{

			 // update dodecahedrons
			{
				evaluate_flying_units(entity_manager, dt);
			}


			{
				if (!count_by_type(entity_manager, Entity_Type::Cube)) 
				{
					play_sound("applause");
					done = true;
				}
			}
			// at this point, we can start rendering static geometry.
			// collision_check()

	
				if (input.mouse_right) play_sound_3d("chicken");
				if (input.mouse_left)  play_sound("plop_shorter_runup");
		}

	}
}


// call once
void game_init()
{


}
