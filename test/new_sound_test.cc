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
	
	// uint32_t buffer = msound::register_wav_file("chicken.wav");
	// msound::buffer_to_source(buffer);

	uint32_t golfbuffer = msound::register_wav_file("golf_ball_putt_in.wav");
	msound::buffer_to_source(golfbuffer);

	uint32_t musicbuffer = msound::register_wav_file("music_16khz.wav");
	msound::buffer_to_source(musicbuffer);

	msound::play_sound("music_16khz.wav");

	// msound::play_music("music_16khz.wav");


	// msound::pause_music();
	// msound::resume_music();



	while(true) {};
}


