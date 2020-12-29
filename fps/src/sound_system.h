#ifndef INCLUDED_SOUND_SYSTEM_
#define INCLUDED_SOUND_SYSTEM_
#include <string>

// once
void init_sound_system();
void load_sound(const std::string& sound_name);
void deinit_sound_system();

// whenever
void play_sound_at_position(const char* sound_name, const float x, const float y, const float z);
void play_sound_3d(const char* sound_name);
void play_sound(const char* sound_name,
		 const float volume = 1.f, // Full volume 
         const float pan = 0.f,    // Centered
         const bool paused = false,      // Not paused
         const int bus = 0); // primary bus

// this sound will not be culled if the number of sound effects is reached.
void play_sound_safe(const char* sound_name);


void set_source_attenuation(const char* sound_name);




// every_frame
void update_listener(float pos_x, float pos_y, float pos_z,
					 float front_x, float front_y, float front_z,
					 float up_x, float up_y, float up_z, 
					 float v_x, float v_y,  float v_z);
void update_attenuation();





#endif