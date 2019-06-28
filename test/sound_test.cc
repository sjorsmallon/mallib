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

using std::cout;
using std::cerr;

int main()
{
	Sound_Device sound_device;
	auto wav_file = msound::load_wav_file("chicken.wav");
	const uint32_t source = sound_device.data_to_buffer(wav_file);
	sound_device.play_sound(source);

	while(true) {};
}


