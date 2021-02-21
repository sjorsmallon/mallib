#ifndef INCLUDED_SOUND_SYSTEM_
#define INCLUDED_SOUND_SYSTEM_
#include <string>
#include <glm/vec3.hpp>

// once
void init_sound_system();
void load_sound(const std::string& sound_name);
void deinit_sound_system();

// whenever 
void set_sound_path(const char* relative_path_to_main_source);

void play_sound_at_position(const char* sound_name, const float x, const float y, const float z);
void play_sound_3d(const char* sound_name);
void play_sound(
    const char* sound_name,
    const float volume = 1.f, // Full volume 
    const float pan = 0.f,    // Centered
    const bool paused = false,      // Not paused
    const int bus = 0); // primary bus

// this sound will not be culled if the number of sound effects is reached.
void play_sound_safe(const char* sound_name);


void set_source_attenuation(const char* sound_name);




// every_frame
void update_listener(
    glm::vec3 listener_position,
    glm::vec3 front,
    glm::vec3 up,
    glm::vec3 velocity);

void update_attenuation();





#endif