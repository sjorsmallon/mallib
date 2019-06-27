#include "sound_device.h"
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif __linux
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <iostream>
#include <string>
#include <fstream>

using std::cin;
using std::cout;
using std::cerr;
using std::fstream;
using std::string;


// find the file size
int msound::get_file_size(FILE* inFile)
{
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}

Wav_File msound::load_wav_file(const char* filename)
{
	Wav_Header wav_header;
	Wav_File file_to_return;
	const int header_size = sizeof(wav_header);
	const char* file_path = "chicken.wav";

	FILE* wav_file = fopen(file_path, "r");
	if (wav_file == nullptr)
	{
	    fprintf(stderr, "Unable to open wav file: %s\n", file_path);
	    return {};
	}
	const size_t filelength = get_file_size(wav_file);

	//Read the header
	const size_t bytes_read = fread(&wav_header, 1, header_size, wav_file);
	if (bytes_read > 0)
	{
	    //Read the data
	    uint16_t bytes_per_sample = wav_header.bits_per_sample / 8;      //Number     of bytes per sample
	    uint64_t num_samples = wav_header.chunk_size / bytes_per_sample; //How many samples are in the wav file?
	    const int filelength = msound::get_file_size(wav_file);
  		const size_t buffer_size = filelength - sizeof(wav_header);

  		file_to_return.header = wav_header;
  		file_to_return.data.push_back({}); // one channel.
  		file_to_return.data[0].resize(buffer_size);

	    const size_t data_bytes_read = fread(file_to_return.data[0].data(), sizeof(uint8_t), buffer_size / sizeof(uint8_t), wav_file);
	    if (data_bytes_read > 0)
	    {
	    	// cout << "read " << data_bytes_read << "bytes" << '\n';
	    }
	    else
	    {
	    	cout << "read 0 bytes in sound_device. exiting.." << '\n';
    	}

    }
    fclose(wav_file);
    return file_to_return;
}


Sound_Device::Sound_Device()
//:
{
	m_device = alcOpenDevice(nullptr);
	if (m_device)
	{
		m_context = alcCreateContext(m_device, nullptr);
		alcMakeContextCurrent(m_context);
	}

	//@Move: separate the data here.
	m_num_buffers = 1; 
	m_buffers.reserve(m_num_buffers);

	alGenBuffers(m_num_buffers, buffers.data());
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("errortjeee");
		// DisplayALError("alGenBuffers: ", error);
	}

	//@Incomplete: this is 32 sound sources.
	m_num_sound_sources = 32;
	m_sound_sources.reserve(m_num_sound_sources);

	alGenSources(m_num_sound_sources, m_sound_sources.data());

	// bind buffers to sound sources.
	alSourcei(sound_source, AL_BUFFER, buffers[0]);
}




Sound_Device::~Sound_Device()
{
	alDeleteSources(m_num_sound_sources, m_sound_sources.data());
    alDeleteBuffers(m_num_buffers, m_buffers.data());
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

// void Sound_Device::load_wav_file(const char* filename)
// {
// 	// format, data , size, freq, loop?
// 	// g_Buffers & num_buffers
// 	// error?
// 	loadWAVFile(filename &format, &data, &size, &freq, &loop);

// 	if ((error = alGetError()) = AL_NO_ERROR)
// 	{
// 		DisplayAlError("aluitloadWAVFILe error:", error);
// 		alDeleteBuffers(NUM_BUFFERS, g_Buffers);
// 		return;
// 	}

// 	alBufferData(g_Buffers[0], format, data, size, freq);
// 	if ((error = alGetError()) != AL_NO_ERROR)
// 	{
// 		DisplayAlError("alutLoadWavFILE error:", error);
// 		alDeleteBuffers(NUM_BUFFERS, g_Buffers);
// 		return;
// 	}
// }

void Sound_Device::play_sound(const char* filename)
{ 
    
}

void Sound_Device::play_music(const char* filename)
{

}
