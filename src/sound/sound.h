#ifndef INCLUDED_SOUND_
#define INCLUDED_SOUND_

#include <soloud/soloud.h>
#include <soloud/soloud_thread.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>
#include <stdint.h> // uint32_t
#include <string>
#include <memory>
#include <map>	  // music(); // we want to store music w.r.t filenames 
#include <vector> // active_sounds

using Handle = uint32_t;
using wav_file  = SoLoud::Wav;
using wav_stream = SoLoud::WavStream;


namespace sound
{

	// hmm. the terms of use here are ill-defined.
	struct Sound_Settings
	{
		std::string sound_folder;
		int reserved_voices;
		int max_voices;
	};

	
	void init_sound();


	struct Music
	{
		int handle;
		//@Memory: for now, this is a unique ptr
		// we want to not have ownership actually,
		// but just let the allocator know that the memory can be used again.
		// however, let's first make this work.
		// In retrospect, this is a terrible idea,
		// since it's not the whole file, but a stream.
		// anyway. 
		std::unique_ptr<wav_stream> stream_ptr;
	};

	struct Sound
	{
		int handle;
		wav_file* wav_ptr;
	};

	using Active_Music = uint32_t;

	//@Refactor: move to globals(?). we can keep it "living" in the namespace,
	// for the sake of the module?
	SoLoud::Soloud& soloud(); 
	Active_Music& active_music();
	std::map<std::string, Music>& music();

	// Ring_Buffer<wav_stream *>& active_sounds(); 
	std::vector<Sound>& active_sounds();


	void play_sound(std::string sound);
	void load_music(std::string music);
	void play_music(std::string music);
	void stop_music();


	//@Refactor: for if we want to move active_music to global game state.
	void toggle_pause_music(Active_Music active_music, bool pause); // only music. 
	void toggle_pause_all(bool pause); // music & sound fx.


	void perform_shutdown();
	// what does this mean?
	void update_audio(); 
}


#endif