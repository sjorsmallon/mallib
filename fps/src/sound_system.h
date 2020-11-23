#ifndef INCLUDED_SOUND_SYSTEM_
#define INCLUDED_SOUND_SYSTEM_
#include <string>

void init_sound_system();

void load_sound(const std::string& sound_name);
void play_sound(const char* sound_name);


#endif