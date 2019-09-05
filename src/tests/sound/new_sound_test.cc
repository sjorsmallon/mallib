#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif __linux
#include <AL/al.h>
#include <AL/alc.h>
#include <cstdio>
#endif
#include <iostream>


#include "../msound/msound.h"

using std::cout;
using std::cerr;

int main()
{	

	uint32_t musicbuffer = msound::register_wav_file("music_exported.wav");
	msound::buffer_to_source(musicbuffer);
	msound::play_sound("music_exported.wav");

	// msound::play_music("music_16khz.wav");
	// msound::pause_music();
	// msound::resume_music();

	while(true) {};
}


