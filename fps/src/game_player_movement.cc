#include "game.h"
#include <glm/vec3.hpp>
#include "input.h"

constexpr const int KEY_SPACE = 32;
constexpr const int KEY_W = 87;
constexpr const int KEY_A = 65;
constexpr const int KEY_S = 83;
constexpr const int KEY_D = 68;
constexpr const int KEY_P = 80;
constexpr const int KEY_O = 79;
constexpr const int KEY_I = 73;

namespace 
{
    // pm
    // quake constants
    
    float pm_ground_acceleration = 10.f;
    float pm_air_acceleration = 10.0f; // was 1.0f;
    float pm_friction = 6.f;
    float pm_stopspeed = 100.0f;
    float pm_maxvelocity = 320.0f;
    float pm_jumpvelocity = 270.0f;


    float pm_noclip_velocity = 0.2f;


    // world
    float g_gravity = 800.0f;

    // input
    float input_axial_extreme = 127.0f; // -127, 127 for forward and backward (for controller support)

    float CLIP_OVERCLIP = 1.001f;



    [[nodiscard]]
    glm::vec3 apply_friction(glm::vec3 old_movement_vector, bool grounded, bool jump_pressed_this_frame, float dt)
    {
        if (grounded) old_movement_vector.y = 0.0f;

        float velocity = glm::length(old_movement_vector);

        //@Cleanup(Sjors): fix the velocity flooring threshold.
        const float velocity_treshold = 1.0f;
        if (velocity < velocity_treshold)
        {
            return glm::vec3(0.0f);
        }

        float velocity_drop = 0.0f;

        if (grounded && !jump_pressed_this_frame) // && are_currently_knockbacked
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
        //  logr::report_warning("opposite directions pressed..\n");
        //  return glm::vec3(0.0f);
        // }

        // logr::report("result: {}\n", result);
        // if (adjusted_velocity < 0.0000001f)
        // {
        //  logr::report_warning("adjusted_vector velocity too small, returning acceleration vector\n");
        //  return acceleration_speed * wish_direction;
        // }

        return result;
    }   

    // if we succeeded and did not clip it, ....
    // glm::vec3 slide_move(const glm::vec3 old_position, const glm::vec3 movement_vector, const bool player_is_airborne, const float dt)
    // {
    //  if (player_is_airborne)
    //  {
    //      glm::vec3 new_movement_vector = movement_vector;
    //      new_movement_vector.y -= g_gravity * dt;
    //  }

    //  glm::vec3 position = old_position + (new_movement_vector * dt);
    //  return position;
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
            new_movement_vector.y -= g_gravity * dt;
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
}


// @dependencies:
// g_camera_velocity
glm::vec3 noclip_move(
    const Input& input,
    const glm::vec3 old_position,
    const glm::vec3 front,
    const glm::vec3 right,
    const glm::vec3 up,
    const float dt)
{
    glm::vec3 position = old_position;

    if (input.keyboard_state[KEY_W]) position = position + (front * pm_noclip_velocity * dt);
    if (input.keyboard_state[KEY_S]) position = position - (front * pm_noclip_velocity * dt);
    if (input.keyboard_state[KEY_A]) position = position - (right * pm_noclip_velocity * dt);
    if (input.keyboard_state[KEY_D]) position = position + (right * pm_noclip_velocity * dt);

    if (input.keyboard_state[KEY_SPACE]) position = position + (up * pm_noclip_velocity * dt);

    return position;
}




std::tuple<glm::vec3, glm::vec3> air_move(
    const Input& input,
    const glm::vec3 old_position,
    const glm::vec3 old_movement_vector,
    const glm::vec3 front,
    const glm::vec3 right,
    const float dt)
{
    // logr::report_warning("[air_move]\n");
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

    glm::vec3 movement_vector = glm::vec3(0.0f);

    if (wish_velocity < 0.1f) 
    {
        //@Note(Sjors) don't normalize: will yield nan or inf
        movement_vector = adjusted_movement_vector;
    }
    else
    {
        // Take into account whether we need air value or not.
        movement_vector = accelerate(adjusted_movement_vector, wish_direction, wish_velocity, pm_air_acceleration, dt);
    }

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

    // The switch to air movement occurs before this. If we are in the air, we should never end up here.
    // this means that we are always grounded.

    grounded = true;
    // apply friction
    glm::vec3 adjusted_movement_vector = apply_friction(old_movement_vector, grounded, jump_pressed_this_frame, dt);

    float forward_input = input_axial_extreme * input.keyboard_state[KEY_W] - input_axial_extreme * input.keyboard_state[KEY_S];
    float right_input   = input_axial_extreme * input.keyboard_state[KEY_D] - input_axial_extreme * input.keyboard_state[KEY_A];
    float up_input      = input_axial_extreme * (jump_pressed_this_frame && grounded);

    // input scale is dependent on axial input. Keyboard keys always provide maximum input.
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

    // normalize input vector, but keep velocity! this HAS to be normalized by dt later.
    if (received_input)
    {
        wish_velocity  = scale * glm::length(wish_direction);
        wish_direction = glm::normalize(wish_direction);
    } 

    // if we are ducking
    // if (player_state & PMF_DUCKED) wish_speed = pm_maxspeed * pm_duckscale (pm_duckscale = 0.5);
    
    glm::vec3 movement_vector = glm::vec3(0.0f);

    if (wish_velocity < 0.1f) 
    {
        //@Note(Sjors) don't normalize: will yield nan or inf
        movement_vector = adjusted_movement_vector;
        // logr::report("no input!\n");
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