#include "msound.h"
#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include "../file_handler/file_handler.h"
#include "../sound_device/sound_device.h"
using std::cerr;



namespace msound
{
	Sound_Device sound_device;
	std::map<std::string, Wav_File> wav_files;
    std::map<std::string, uint32_t> sound_sources; 
    std::map<std::string, uint32_t> buffers;
    std::map<uint32_t, uint32_t> buffer_to_sources;

	std::map<std::string, std::vector<uint32_t>> music_buffers;
}

// 
//	load wav file,
// 	bind to a buffer,
//	bind to source.
//
uint32_t msound::register_wav_file(const char* soundname)
{
	// register the file in all the lookup maps.

	msound::wav_files.emplace( soundname, std::move( msound::load_wav_file( soundname ) ) );
	uint32_t buffer = sound_device.data_to_buffer(msound::wav_files[soundname]);
	msound::buffers.emplace(soundname, buffer);

	// msound::sound_sources.emplace(soundname, )
	return buffer;
}

uint32_t msound::buffer_to_source(const uint32_t buffer)
{
	uint32_t source = sound_device.buffer_to_source(buffer);
	msound::buffer_to_sources.emplace(buffer, source);
	return source;
}
	


void msound::play_sound( const char* soundname )
{
	//@Todo:
	// is the file bound to a buffer?
	// uint32_t buffer = msound::buffers[soundname];
	// if buffer == 0 it is default constructed, so remove.
	uint32_t buffer = msound::buffers.at(soundname);
	uint32_t source = msound::buffer_to_sources.at(buffer);

	sound_device.play_sound(source);
}


Wav_File msound::load_wav_file(const char* filename)
{
	Wav_Header wav_header;
	const int32_t header_size = sizeof(wav_header);
	File_Handler file_handler(filename);
	const int32_t filelength = file_handler.file_size();

	Wav_File file_to_return = {};

	//read the header 
	const size_t bytes_read = fread(&wav_header, 1, header_size, file_handler.file());

	if (bytes_read > 0)
	{
		cerr << wav_header.sub_chunk_2_ID[0]  << '\n';

		
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
			else if  (wav_header.num_channels == 2)
			{
				cerr << "stereo\n";
				file_to_return.format = AL_FORMAT_STEREO16;
			}
		}

		cerr << "sub chunk size: " <<  wav_header.sub_chunk_2_size << '\n'; //@Bug: is this little or big endian?

		//@TODO: remove stereo stuff, not necessary (i think?)
		file_to_return.header = wav_header;
		file_to_return.data.push_back({});
		uint32_t buffer_size = filelength - sizeof(wav_header);
		cerr << "buffer size: " << buffer_size << '\n';
		file_to_return.data[0].resize(wav_header.sub_chunk_2_size);			

		const size_t data_bytes_read = fread(file_to_return.data[0].data(), sizeof(uint8_t), wav_header.sub_chunk_2_size, file_handler.file());
	    if (data_bytes_read > 0)
	    {
	    	cerr << "read " << data_bytes_read << " bytes" << '\n';
	    }
	    else
	    {
	    	cerr << "read 0 bytes in sound_device. exiting.." << '\n';
    	}
	}
	return file_to_return; //@Leak: this can be empty.
}
















