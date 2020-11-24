#ifndef INCLUDED_SOUND_SYSTEM_
#define INCLUDED_SOUND_SYSTEM_
#include <string>

void init_sound_system();

void load_sound(const std::string& sound_name);
void play_sound(const char* sound_name);
void play_sound_3d(const char* sound_name);

void set_source_attenuation(const char* sound_name);

void update_listener(float pos_x, float pos_y, float pos_z,
					 float front_x, float front_y, float front_z,
					 float up_x, float up_y, float up_z, 
					 float v_x, float v_y,  float v_z);
#endif