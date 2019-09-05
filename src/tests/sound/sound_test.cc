#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif __linux
#include <AL/al.h>
#include <AL/alc.h>
#include <cstdio>
#endif
#include <iostream>

#include "../sound/sound_device.h"
#include "../string/string.cc"

using std::cout;
using std::cerr;

int main()
{	

	Sound_Device sound_device;
	// SString chicken = "chicken.wav";
	std::string chicken = "chicken.wav";

	//msound::register_wav_file("chicken.wav");

	auto wav_file = msound::load_wav_file("chicken.wav");


	msound::register_wav_file("chicken.wav");
	


	const uint32_t buffer = sound_device.data_to_buffer(wav_file);
	const uint32_t source = sound_device.buffer_to_source(buffer);



	sound_device.play_sound(source);
	
	while(true) {};
}


