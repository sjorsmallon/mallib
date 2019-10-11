#include "sound.h"
#include <string>
#include "fmt/core.h"


using Active_Music = uint32_t;

std::map<std::string, sound::Music>& sound::music()
{
	static std::map<std::string, sound::Music> music;
	return music;
}

sound::Active_Music& sound::active_music()
{
	static Active_Music active_music;
	return active_music;
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
	//@Note: if any music still playing:
	if (sound::soloud().getActiveVoiceCount() > 0)
  	{
    	// Still going, sleep for a bit
    	// SoLoud::Thread::sleep(100);
 	}

}

void sound::load_music(std::string filename)
{
	fmt::print("[warning] load_music does not actually load to memory! it just stores a file handle.\n");
	auto& sound_player = sound::soloud();
	//@Memory: think about what we want to do here. This is currently streaming from file.
	// the implementation of WavStream is "new"ing room and has a destructor.
	// but do we want the music to be deleted or not? 
	// do we just load whenever we need?
	// or do we keep references?
	sound::Music music_wrapper = {};
	music_wrapper.stream_ptr = std::make_unique<SoLoud::WavStream>();
	SoLoud::WavStream* music = music_wrapper.stream_ptr.get();

	int result = music->load(filename.c_str());
	fmt::print("load_music soloud error: {}\n", sound_player.getErrorString(result));

	// save the music wrapper & transfer ownership to the map.
	sound::music().insert(std::pair<std::string,sound::Music>(filename, std::move(music_wrapper))); 
}


void sound::play_music(std::string filename)
{
	auto& sound_player = sound::soloud();
	// if this does not exist, do we load it dynamically?
	auto& music_wrapper = sound::music()[filename];

	SoLoud::WavStream* music = music_wrapper.stream_ptr.get();

	uint32_t handle = sound_player.play(*music);
	music_wrapper.handle = handle;

	// set active music.
	auto& active_music = sound::active_music();
	active_music = handle;


}

void sound::play_sound(std::string filename)
{

	auto& sound_player = sound::soloud();
	SoLoud::Wav sample;
	int result = sample.load(filename.c_str());
	fmt::print("play_sound soloud error: {}\n", sound_player.getErrorString(result));
	uint32_t handle = sound_player.play(sample);
	if (sound_player.getActiveVoiceCount() > 0)
  	{
    	// Still going, sleep for a bit
    	SoLoud::Thread::sleep(100);
 	}
}



//@Refactor: toggle_pause?
void sound::pause_music(Active_Music active_music)
{
	auto &sound_player = sound::soloud();
	sound_player.setPause(active_music, true);
}

void sound::resume_music(Active_Music active_music)
{
	// resume playing
	auto &sound_player = sound::soloud();
	sound_player.setPause(active_music, false);
}

void sound::stop_music()
{

	sound::soloud().stop(sound::active_music()); 
	// sound::active_music().stop();

}