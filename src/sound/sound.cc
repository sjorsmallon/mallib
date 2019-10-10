#include "sound.h"
#include <string>
#include "fmt/core.h"

using Active_Music = uint32_t;

sound::Active_Music& sound::active_music()
{
	static Active_Music music;
	return music;
}

SoLoud::Soloud& sound::soloud()
{
	static SoLoud::Soloud so_loud;
	return so_loud;
}

void sound::init_sound()
{
	sound::soloud().init();
}

void sound::perform_shutdown()
{
	sound::soloud().deinit();
}

void sound::update_audio()
{
	//@Note: is this necessary?
	if (sound::soloud().getActiveVoiceCount() > 0)
  	{
    	// Still going, sleep for a bit
    	SoLoud::Thread::sleep(100);
 	}
}


void sound::play_music(std::string filename)
{
	auto& sound_player = sound::soloud();
	//@FIXME::: this is a memory leak. I just want to see whether or not if we allocate
	// it on the heap, it works.
	SoLoud::WavStream* music = new SoLoud::WavStream;

	// SoLoud::WavStream music; // does this wavstream disappear out of scope? that sort of sucks. 
	// int result = music.load(filename.c_str());
	int result = music->load(filename.c_str());

	
	fmt::print("play_music soloud error: {}\n", sound_player.getErrorString(result));

	// uint32_t handle = sound_player.play(music);
	uint32_t handle = sound_player.play(*music);

	// set active music.
	auto& active_music = sound::active_music();
	active_music = handle;
	// while (sound_player.getActiveVoiceCount() > 0)
 //  	{
 //    	// Still going, sleep for a bit
 //    	SoLoud::Thread::sleep(100);
 // 	}

}

void sound::play_sound(std::string filename)
{
	auto& sound_player = sound::soloud();
	SoLoud::Wav sample;
	int result = sample.load(filename.c_str());


	fmt::print("play_sound soloud error: {}\n", sound::soloud().getErrorString(result));

	uint32_t handle = sound_player.play(sample);
	if (sound_player.getActiveVoiceCount() > 0)
  	{
    	// Still going, sleep for a bit
    	SoLoud::Thread::sleep(100);
 	}
}


void sound::pause_music(Active_Music active_music)
{
}

void sound::stop_music()
{

	sound::soloud().stop(sound::active_music()); 
	// sound::active_music().stop();

}