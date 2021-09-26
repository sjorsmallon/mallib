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
constexpr const int KEY_1 = 48;
constexpr const int KEY_2 = 49;
constexpr const int KEY_3 = 50;
constexpr const int KEY_4 = 51;
constexpr const int KEY_5 = 52;
constexpr const int KEY_6 = 53;
constexpr const int KEY_7 = 54;
constexpr const int KEY_8 = 55;
constexpr const int KEY_9 = 56;

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
    // ---------------
    // cvars
    // ---------------

    // controls
    float g_mouse_sensitivity = 0.08f;

    // flying units
    float g_dodecahedron_velocity = 70.0f;
    float g_wanted_distance = 80.0f;
    float g_wanted_height = 50.0f;
    float g_focus = 1.f;
    float g_alignment = 1.f;
    float g_cohesion = 1.f;



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
        
        new_camera.front.x = cos(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
        new_camera.front.y = sin(glm::radians(new_camera.pitch));
        new_camera.front.z = sin(glm::radians(new_camera.yaw)) * cos(glm::radians(new_camera.pitch));
        new_camera.front = glm::normalize(new_camera.front);
        
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

    void evaluate_flying_units(Entity_Manager& entity_manager, const float dt)
    {
        timed_function("evaluate_flying_units");
        auto& guy = get_main_player_entity(entity_manager);

        for (auto&& entity: by_type(entity_manager, Entity_Type::Cube))
        {
            glm::vec3 direction_vector =  glm::normalize(guy.position - entity.position);
            entity.position = entity.position + (direction_vector);  
            entity.movement_vector = direction_vector;
        }
    }

    void evaluate_shot(Entity_Manager& entity_manager, Camera camera)
    {
        timed_function("evaluate_shot");
        const float sphere_radius = 20.f;

        for(auto&& entity: by_type(entity_manager, Entity_Type::Cube))
        {
            if (entity.scheduled_for_destruction) continue;

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

    //@FIXME(Sjors): fix this, since we're currently not waiting for a frame.
    // if (clamped_dt < FRAMETIME_1000_FPS)
    // {
    //     clamped_dt = FRAMETIME_1000_FPS;    
    // }
    if (clamped_dt > FRAMETIME_10_FPS)      clamped_dt = FRAMETIME_10_FPS;      

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
        auto& player = get_main_player_entity(entity_manager); 

        static bool all_enemies_killed = false;
        if (!all_enemies_killed) 
        {
            // BEFORE MOVING ANYTHING, check shot intersection, since we clicked at the position we _are_ in
            // and target that _are_ in a particular position
            if (input.mouse_left) evaluate_shot(entity_manager, game_state.camera);
            //@Note(Sjors): it is imperative that the player gets updated first, since that 
            // is the bottleneck we have to be dealing with in the next frames. (after, we can go wide).
        }
            // update player and camera.
            {
                game_state.camera = update_camera_entity(input, game_state.camera, clamped_dt);
                player.position = game_state.camera.position;
            }

            // update sound system
            {
                update_listener(
                    player.position,
                    game_state.camera.front,
                    game_state.camera.up,
                    game_state.camera.movement_vector
                );
            }
            
        if (!all_enemies_killed)
        {

             // update flying units
            {
                evaluate_flying_units(entity_manager, dt);
            }

            // check if we're actually done.      
            {
                if (!count_by_type(entity_manager, Entity_Type::Cube)) 
                {
                    play_sound("applause");
                    all_enemies_killed = true;
                }
            }

            // at this point, we can start rendering static geometry.
            // collision_check()
            if (input.mouse_right)
            {
                // fire_rocket_wrapper();
            }
            if (input.mouse_left)  play_sound("weapons/bananade/grenade_thoomp");
         }

    }
}


