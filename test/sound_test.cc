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
	auto wav_file = msound::load_wav_file("i_feel_so_alive.wav");
	const uint32_t buffer = sound_device.data_to_buffer(wav_file);
	const uint32_t source = sound_device.buffer_to_source(buffer);
	sound_device.play_sound(source);

	while(true) {};
}


