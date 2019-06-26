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
	Wav_File wav_file = msound::load_wav_file("test");
	auto Device = alcOpenDevice(nullptr);
	if (Device)
	{
		auto Context = alcCreateContext(Device, nullptr);
		alcMakeContextCurrent(Context);
	}

	ALenum error;
	const int num_buffers = 1;
	std::vector<ALuint> buffers(1);

	alGenBuffers(num_buffers, buffers.data());

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("errortjeee");
		// DisplayALError("alGenBuffers: ", error);
	}


	alBufferData(buffers[0],  AL_FORMAT_MONO8, wav_file.data[0].data(), wav_file.data[0].size() * sizeof(uint8_t), wav_file.header.samples_per_sec);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		// DisplayAlError("alutLoadWavFILE error:", error);

		alDeleteBuffers(num_buffers, buffers.data());
		exit(1);
	}
	cerr << "no error in alBufferData";

	ALuint sound_source;
	alGenSources(1, &sound_source);
	alSourcei(sound_source, AL_BUFFER, buffers[0]);
	alSourcei(sound_source, AL_LOOPING, AL_TRUE);

	alSourcePlay(sound_source);



}



// int main()
// {
// 	const int NUM_BUFFERS = 1;
// 	ALuint g_buffers[NUM_BUFFERS];
// 	ALenum format;
// 	ALsizei size;
// 	ALsizei freq;
// 	ALenum error;
// 	bool loop = false;
// 	unsigned char* data; 

// 	auto Device = alcOpenDevice(nullptr);
// 	if (Device)
// 	{
// 		auto Context = alcCreateContext(Device, nullptr);
// 		alcMakeContextCurrent(Context);
// 	}

// 	// g_bEAX = alIsExtensionPresent("EAX2.0");
// 	// alGetError();

// 	alGenBuffers(NUM_BUFFERS, &g_buffers[0]);

// 	if ((error = alGetError()) != AL_NO_ERROR)
// 	{
// 		printf("errortjeee");
// 		// DisplayALError("alGenBuffers: ", error);
// 	}

// 	// format, data , size, freq, loop?
// 	// g_Buffers & num_buffers
// 	// error?


// 	loadWAVFile("chicken.wav", &format, &data, &size, &freq, &loop);

// 	if ((error = alGetError()) = AL_NO_ERROR)
// 	{
// 		DisplayAlError("aluitloadWAVFILe error:", error);
// 		alDeleteBuffers(NUM_BUFFERS, g_buffers);
// 		return;
// 	}

// 	// alBufferData(g_Buffers[0], format, data, size, freq);
// 	// if ((error = alGetError()) != AL_NO_ERROR)
// 	// {
// 	// 	DisplayAlError("alutLoadWavFILE error:", error);
// 	// 	alDeleteBuffers(NUM_BUFFERS, g_Buffers);
// 	// 	return;
// 	// }
// }
