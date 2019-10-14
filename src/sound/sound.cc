#include "sound.h"
#include <string>
#include "fmt/core.h"
#include <algorithm>


std::vector<sound::Sound>& sound::active_sounds()
{
	static std::vector<sound::Sound> active_sounds;
	return active_sounds;
}


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
	//@FIXME: music is never freed. we should do that.

	//@Note: if any music still playing:
	// if (sound::soloud().getActiveVoiceCount() > 0)
 //  	{
 //    	// Still going, sleep for a bit
 //    	// SoLoud::Thread::sleep(100);
 // 	}

	//@Speed: think about how to do this better. we Iterate
	// over all sounds instead of only over the done ones.
	//@Memory: we are deleting the sounds now.
	auto& sounds = sound::active_sounds();
	for (auto sound_ptr = sounds.begin(); sound_ptr != sounds.end();)
	{
		if(!sound::soloud().isValidVoiceHandle(sound_ptr->handle))
		{
			// delete *it;   // delete wav -> new wav in play_sound
			fmt::print("[sound] found invalid voice handle. deleting it.\n");
			free(sound_ptr->wav_ptr); // free wav_ptr
			sound_ptr = sounds.erase(sound_ptr);
		}
		else ++sound_ptr;
	}
}

void sound::load_music(std::string filename)
{
	fmt::print("[sound] WARNING::load_music does not actually load to memory! it just stores a file handle.\n");
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
	fmt::print("[sound] load_music soloud error: {}\n", sound_player.getErrorString(result));

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
	//@Memory: replace this with placement new? but then we need to know the size of the file....
	// now we're newing and freeing all over the place. use placement new.
	sound::Sound sound_wrapper = {};
	sound_wrapper.wav_ptr = new wav_file();  // new wav_ptr
	//@Speed:
	SoLoud::Wav& sample = *sound_wrapper.wav_ptr;

	int result = sample.load(filename.c_str());
	fmt::print("[sound] play_sound soloud error: {}\n", sound_player.getErrorString(result));
	sound_wrapper.handle = sound_player.play(sample);
	sound::active_sounds().push_back(sound_wrapper);
}

//@Refactor: toggle_pause?
void sound::toggle_pause_music(Active_Music active_music, bool pause)
{
	auto &sound_player = sound::soloud();
	sound_player.setPause(active_music, pause);
}

void sound::toggle_pause_all(bool pause)
{
	auto &sound_player= sound::soloud();
	sound_player.setPauseAll(pause);
}

void sound::stop_music()
{

	sound::soloud().stop(sound::active_music()); 
	// sound::active_music().stop();

}