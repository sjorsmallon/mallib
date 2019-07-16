#include "msound.h"
#include "../shared/shared.h"


void msound::register_wav_file(const char* soundname)
{
	// register the file in all the lookup maps.

	msound::wav_files.emplace( soundname, move( msound::load_wav_file( soundname ) ) );
	uint32_t buffer = sound_device.data_to_buffer(msound::wav_files[soundname]);
	msound::buffers.emplace(soundname, buffer);

	// msound::sound_sources.emplace(soundname, )


}



void msound::play_sound( const char* soundname )
{

}



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





