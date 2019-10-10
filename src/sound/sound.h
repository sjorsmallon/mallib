#ifndef INCLUDED_SOUND_
#define INCLUDED_SOUND_

#include <soloud/soloud.h>
#include <soloud/soloud_thread.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>
#include <stdint.h> // uint32_t
#include <string>


using Handle = uint32_t;
using wav_file  = SoLoud::Wav;


namespace sound
{
	using Active_Music = uint32_t;

	// Handle Active_Music = {};
	SoLoud::Soloud& soloud(); 

	Active_Music& active_music();

	void play_sound(std::string sound);
	void play_music(std::string music);
	void stop_music();
	//@Refactor: for if we want to move active_music to global game state.
	void pause_music(Active_Music active_music);

	void init_sound();
	void perform_shutdown();
	void update_audio(); 
}


#endif