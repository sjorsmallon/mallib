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
	// @Incomplete: initially 1:1? do we need to keep track of which 
	// buffer is bound to what source? or what buffers are actually filled?
	// are all buffers always filled? are there as many buffers as there
	// are sound files? or are they streamed in? help

	// for (size_t idx = 0; idx != m_num_sound_sources; ++idx)
	// 	alSourcei(m_sound_sources[idx], AL_BUFFER, m_buffers[idx]);


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
	cerr << "buffer_ID: " << buffer_ID;
	size_t size = wav_file.data[0].size() * sizeof(uint8_t);

	if (wav_file.format == AL_FORMAT_MONO16 || wav_file.format == AL_FORMAT_STEREO16)
		size /= 2; 	
	cerr << "my size: " << size << ", wav_file.size" << wav_file.header.sub_chunk_2_size;


	alBufferData(buffer_ID,  wav_file.format, wav_file.data[0].data(), wav_file.data[0].size() * sizeof(uint8_t), wav_file.header.samples_per_sec);
	if ((m_error = alGetError()) != AL_NO_ERROR)
	{
		if (m_error == AL_INVALID_VALUE)
		{
			cerr << "albufferData error: invalid value"; 
		}

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
	m_buffers_occupied[idx] = 1;	
	return m_buffers[idx];
}

ALuint Sound_Device::find_free_source()
{
	auto result = std::find(m_sound_sources_occupied.begin(), m_sound_sources_occupied.end(), 0);
	size_t idx = std::distance(m_sound_sources_occupied.begin(), result);
	m_sound_sources_occupied[idx] = 1;
	return m_sound_sources[idx];
}


// find the file size

Wav_File msound::load_wav_file(const char* filename)
{
	Wav_Header wav_header;
	const int header_size = sizeof(wav_header);
	// const char* file_path = "chicken.wav";

	File_Handler file_handler(filename);
	Wav_File file_to_return;
	const int filelength = file_handler.file_size();

	//Read the header
	const size_t bytes_read = fread(&wav_header, 1, header_size, file_handler.file());
	if (bytes_read > 0)
	{
		
		if (wav_header.bits_per_sample == 8)
		{
			cerr << "8 bits\n";
			if (wav_header.num_channels == 1)
			{
				cerr << "mono\n";
				file_to_return.format = AL_FORMAT_MONO8;
			}
			else
			{
				cerr << "stereo\n";
				file_to_return.format = AL_FORMAT_STEREO8;
			}
		}
		else if(wav_header.bits_per_sample == 16)
		{
			cerr << "16 bits\n";
			if (wav_header.num_channels == 1)
			{
				cerr << "mono\n";
				file_to_return.format = AL_FORMAT_MONO16;
			}
			else 
			{
				cerr << "stereo\n";
				file_to_return.format = AL_FORMAT_STEREO16;
			}
		}

	    //Read the data
	    uint16_t bytes_per_sample = wav_header.bits_per_sample / 8;      //Number     of bytes per sample
	    uint64_t num_samples = wav_header.chunk_size / bytes_per_sample; //How many samples are in the wav file?
  		const int buffer_size = filelength - sizeof(wav_header);
  		cerr <<  "buffer size: " << buffer_size << '\n';
  		file_to_return.duration = static_cast<float>(num_samples) / static_cast<float>(wav_header.samples_per_sec);
	    file_to_return.duration *= 1000; //1000 milliseconds in a second.
	    cerr << "file duration: " << file_to_return.duration << '\n';;

  		file_to_return.header = wav_header;
  		file_to_return.data.push_back({}); // one channel.
  		file_to_return.data[0].resize(buffer_size);

	    const size_t data_bytes_read = fread(file_to_return.data[0].data(), sizeof(uint8_t), buffer_size / sizeof(uint8_t), file_handler.file());
	    if (data_bytes_read > 0)
	    {
	    	cout << "read " << data_bytes_read << "bytes" << '\n';
	    }
	    else
	    {
	    	cout << "read 0 bytes in sound_device. exiting.." << '\n';
    	}

    }
    return file_to_return;
}



void Sound_Device::play_sound(const uint32_t sound_source)
{
	alSourcePlay(sound_source);
}

void Sound_Device::play_sound(const char* filename)
{ 
    
}

void Sound_Device::play_music(const char* filename)
{

}
