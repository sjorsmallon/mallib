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
#include "../file_handler/file_handler.h"
#include <algorithm>

using std::cin;
using std::cout;
using std::cerr;
using std::fstream;
using std::string;


Sound_Device::Sound_Device()
{
	m_device = alcOpenDevice(nullptr);
	if (m_device)
	{
		m_context = alcCreateContext(m_device, nullptr);
		alcMakeContextCurrent(m_context);
	}

	//@Move: separate the data here.
	m_num_buffers = 32; 
	m_buffers.reserve(m_num_buffers);
	m_buffers_occupied.assign(m_num_buffers, 0);

	alGenBuffers(m_num_buffers, m_buffers.data());
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenBuffers error");
		alDeleteBuffers(m_num_buffers, m_buffers.data());
		exit(1);
	}

	//@Incomplete: this is 32 sound sources.
	m_num_sound_sources = 32;
	m_sound_sources.reserve(m_num_sound_sources);
	m_sound_sources_occupied.assign(m_num_sound_sources, 0);

	alGenSources(m_num_sound_sources, m_sound_sources.data());
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenSources error");
		alDeleteSources(m_num_sound_sources, m_sound_sources.data());
		exit(1);
	}

	// bind buffers to sound sources.
}


Sound_Device::~Sound_Device()
{
	alDeleteSources(m_num_sound_sources, m_sound_sources.data());
    alDeleteBuffers(m_num_buffers, m_buffers.data());
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

uint32_t Sound_Device::data_to_buffer(const Wav_File& wav_file) // returns source
{
	ALuint buffer_ID = find_free_buffer();	
	cerr << "buffer_ID: " << buffer_ID << '\n';

	// if (wav_file.format == AL_FORMAT_MONO16 || wav_file.format == AL_FORMAT_STEREO16)
	// 	size /= 2; 	
	 // this worked: wav_file.data[0].size() * sizeof(uint8_t) * 2;

	alBufferData(buffer_ID, wav_file.format, wav_file.data[0].data(), wav_file.data[0].size(), wav_file.header.samples_per_sec);
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		if (m_error == AL_INVALID_VALUE)
			cerr << "albufferData error: invalid value." << '\n'; 

	}
	return buffer_ID;
}


uint32_t Sound_Device::buffer_to_source(const uint32_t buffer)
{
	ALuint source_ID = find_free_source();
	cerr << "source ID:  " << source_ID;
	// source_ID = m_sound_sources[0];
	alSourcei(source_ID, AL_BUFFER, buffer);

	return source_ID;
}

ALuint Sound_Device::find_free_buffer()
{
	auto result = std::find(m_buffers_occupied.begin(), m_buffers_occupied.end(), 0);
	size_t idx = std::distance(m_buffers_occupied.begin(), result);
	//@Leak: result can be end.
	m_buffers_occupied[idx] = 1;	// true?
	return m_buffers[idx];
}

ALuint Sound_Device::find_free_source()
{
	auto result = std::find(m_sound_sources_occupied.begin(), m_sound_sources_occupied.end(), 0);
	size_t idx = std::distance(m_sound_sources_occupied.begin(), result);
	//@Leak: result can be end.
	m_sound_sources_occupied[idx] = 1; // true?
	return m_sound_sources[idx];
}


// find the file size



void Sound_Device::play_sound(const uint32_t sound_source)
{
	alSourcePlay(sound_source);
}



void Sound_Device::play_music(const char* filename)
{

}
