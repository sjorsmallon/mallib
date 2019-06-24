<<<<<<< HEAD
#ifndef INCLUDED_SOUND_DEVICE_
#define INCLUDED_SOUND_DEVICE_

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif __linux
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "wav.h"
#include <vector>


#include <iostream>
#include <string>
#include <fstream>


// using this class:
// after invoking openAL function, request the error status
// to see if something went wrong. how we deal with something
// going wrong is something we have to come up with later.

namespace msound
{
	void load_wav_file(const char* filename); //Wav_File 
	int getFileSize(FILE* inFile);
};

class Sound_Device
{
	std::vector<ALuint> m_buffers;
	// actually horrible design, but whatever.
	ALenum error;
	bool m_EAX;

	public:
		Sound_Device();

		void play_sound(const char* filename);
		void play_music(const char* filename);
		void load_wav_file(const char* filename);
};


struct Sound_File
{
	//format, data , size, freq, loop?
	size_t size;
	float freq; 

};
// const int NUM_BUFFERS = 1;
// 	ALuint g_buffers[NUM_BUFFERS];
// 	ALenum error;



struct Wav_Header
{
    // RIFF Chunk Descriptor
    uint8_t         RIFF[4];           // RIFF Header Magic header
    uint32_t        chunk_size;        // RIFF Chunk Size
    uint8_t         WAVE[4];           // WAVE Header
    // "fmt" sub-chunk
    uint8_t         fmt[4];            // FMT header
    uint32_t        sub_chunk_1_size;  // Size of the fmt chunk
    uint16_t        audio_format;      // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        num_of_chan;       // Number of channels 1=Mono 2=Sterio
    uint32_t        samples_per_sec;   // Sampling Frequency in Hz
    uint32_t        bytes_per_sec;     // bytes per second
    uint16_t        block_align;       // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bits_per_sample;   // Number of bits per sample
    // "data" sub-chunk
    uint8_t         sub_chunk_2_ID[4]; // "data"  string
    uint32_t        sub_chunk_2_size;  // Sampled data length
};

struct Wav_File
{
	Wav_Header header;
	uint8_t *data;
	size_t size;
};

#endif


=======
#ifndef INCLUDED_SOUND_DEVICE
#define INCLUDED_SOUND_DEVICE

//@Incomplete: look at openAL


namespace msound
{
    
}


class Sound_Device
{


    public:
        Sound_Device();

        void play_sound(const char* filename);
        void play_music(const char* filename);

};

#endif
>>>>>>> 4caea8383ba20c228c05c28f753e18e7682868df
