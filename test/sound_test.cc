#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif __linux
#include <AL/al.h>
#include <AL/alc.h>
#include <cstdio>
#endif

#include "../sound/sound_device.h"


int main()
{
	msound::load_wav_file("test");
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
