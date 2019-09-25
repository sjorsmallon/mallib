#ifndef INCLUDED_SOUND_
#define INCLUDED_SOUND_

#include <soloud/soloud.h>
#include <soloud/soloud_thread.h>
#include <soloud/soloud_wav.h>
#include <stdint.h>

using Handle = uint32_t;
using wav_file  = SoLoud::Wav;


namespace sound
{
	// Handle Active_Music = {};
	SoLoud::Soloud& soloud(); 

	void init_sound();
	void perform_shutdown();

}


#endif