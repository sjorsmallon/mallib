#include "sound_system.h"
#include <soloud.h>
#include <soloud_wav.h>
#include <map>
#include <string>
#include "logr.h"

namespace
{
	SoLoud::Soloud g_soloud;
	std::map<std::string, SoLoud::Wav> g_samples;
	std::map<std::string, int> g_handles;
	std::map<int, int> g_active_voices; //uh, why is this not an array? :~)

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

void load_sound(const std::string& sound_name_with_extension)
{
	const std::string full_sound_path = sound_path + sound_name_with_extension;

	std::string sound_name = sound_name_with_extension.substr(0, sound_name_with_extension.find("."));
	//@FIXME(Sjors): IMPLICIT CREATION
	auto& sample = g_samples[sound_name];
	sample.load(full_sound_path.c_str());
	// g_soloud.play(sample);

	logr::report("[load_sound] loaded sound: {}\n", sound_name);
}

void play_sound(const char* sound_name,
		 float volume, // Full volume 
         float pan,    // Centered
         bool paused,  // Not paused
         int bus)     // Primary bus
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

void play_sound_at_position(const char* sound_name, const float x, const float y, const float z)
{
	g_handles[sound_name] = g_soloud.play3d(g_samples[sound_name],
		x, // px
		y, // py
		z, // pz
		0.0f, // vx
		0.0f, // vy
		0.0f, // vz
		1.0f, // volume?
		0,
		false);
}



void play_sound_3d(const char* sound_name)
{

	g_handles["chicken"] = g_soloud.play3d(g_samples["chicken"], 
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
	logr::report_warning_once("[set_source_attenuation] NOT IMPLEMENTED!\n");
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
	//@Fixme(Sjors): I would like this to be one.
	constexpr float position_attenuation_factor = 1.f / 1000.f;

	g_soloud.set3dListenerParameters(
		pos_x * position_attenuation_factor, pos_y * position_attenuation_factor, pos_z * position_attenuation_factor, 
		front_x, front_y, front_z, 
 		up_x, up_y, up_z,
 		v_x, v_y, v_z);

	int g_handle = g_handles["chicken"];
	g_soloud.set3dSourceAttenuation(g_handle, 
                           3,
                           1.f);    
	g_soloud.update3dAudio(); // apply change to voices  
}

//@Every_Frame:
void update_attenuation()
{
	int g_handle = g_handles["chicken"];
	// for all playing handles:
	g_soloud.set3dSourceAttenuation(g_handle, 
                           3,
                           1.f);    
	g_soloud.update3dAudio(); // apply change to voices  
}


