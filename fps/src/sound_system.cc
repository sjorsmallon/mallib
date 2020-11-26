#include "sound_system.h"
#include <soloud.h>
#include <soloud_wav.h>
#include <map>
#include <string>

namespace
{
	SoLoud::Soloud g_soloud;
	std::map<std::string, SoLoud::Wav> g_samples;
	std::map<std::string, int> g_handles;
	std::map<int, int> g_active_voices;

	const std::string sound_path = "../assets/sound/";
}

void init_sound_system()
{
	g_soloud.init(SoLoud::Soloud::ENABLE_VISUALIZATION);
}

void deinit_sound_sytem()
{
	g_soloud.deinit();
}

void load_sound(const std::string& sound_name)
{
	const std::string full_sound_path = sound_path + sound_name;
	//@FIXME(Sjors): IMPLICIT CREATION
	auto& sample = g_samples[sound_name];
	sample.load(full_sound_path.c_str());
	g_soloud.play(sample);
}

void play_sound(const char* sound_name,
		 float volume, // Full volume 
         float pan,    // Centered
         bool paused,      // Not paused
         int aBus)        // Primary bus
{
	const int handle = g_soloud.play(g_samples[sound_name]);	
	g_active_voices[handle] = handle;
}

void play_sound_safe(const char* sound_name)
{
	const int handle = g_soloud.play(g_samples[sound_name]);
	g_soloud.setProtectVoice(handle, true);
}

void stop_sound()
{
	g_soloud.stopAll();
}

void play_sound_3d(const char* sound_name)
{
	g_handles["chicken.wav"] = g_soloud.play3d(g_samples["chicken.wav"], 
	              0.0f, 
	              0.0f, 
	              0.0f, 
	              0.0f, 
	              0.0f, 
	              0.0f, 
	              1.0f, 
	              0, 
	              false);

}

void set_source_attenuation(const char* sound_name)
{

	   
}


//@Every_Frame:
void update_listener(float pos_x, 
                     float pos_y, 
                     float pos_z, 
                     float front_x, 
                     float front_y, 
                     float front_z, 
                     float up_x, 
                     float up_y, 
                     float up_z, 
                     float v_x, 
                     float v_y, 
                     float v_z)
{
	g_soloud.set3dListenerParameters(
		pos_x / 1000.0f, pos_y / 1000.0f, pos_z / 1000.0f, 
		front_x, front_y, front_z, 
 		up_x, up_y, up_z,
 		v_x, v_y, v_z);

	int g_handle = g_handles["chicken.wav"];
	g_soloud.set3dSourceAttenuation(g_handle, 
                           3,
                           1.f);    
	g_soloud.update3dAudio(); // apply change to voices  
}

//@Every_Frame:
void update_attenuation()
{
	int g_handle = g_handles["chicken.wav"];
	// for all playing handles:
	g_soloud.set3dSourceAttenuation(g_handle, 
                           3,
                           1.f);    
	g_soloud.update3dAudio(); // apply change to voices  
}


