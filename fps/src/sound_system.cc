#include "sound_system.h"
#include <soloud.h>
#include <soloud_wav.h>


namespace
{
	SoLoud::Soloud g_soloud;
	SoLoud::Wav g_sample;
	const std::string sound_path = "../assets/sound/";

}


void init_sound_system()
{
	g_soloud.init();
}

void load_sound(const std::string& sound_name)
{
	const std::string full_sound_path = sound_path + sound_name;

	g_sample.load(full_sound_path.c_str());
	g_soloud.play(g_sample);
}

void play_sound(const char* sound_name)
{
	g_soloud.play(g_sample);	
}


