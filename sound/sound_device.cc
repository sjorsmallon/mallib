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
int msound::getFileSize(FILE* inFile)
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
	const size_t filelength = getFileSize(wav_file);

	//Read the header
	const size_t bytes_read = fread(&wav_header, 1, header_size, wav_file);
	if (bytes_read > 0)
	{
	    //Read the data
	    uint16_t bytes_per_sample = wav_header.bits_per_sample / 8;      //Number     of bytes per sample
	    uint64_t num_samples = wav_header.chunk_size / bytes_per_sample; //How many samples are in the wav file?

	    cout << "File is                    :" << filelength << " bytes." << '\n';
	    cout << "RIFF header                :" << wav_header.RIFF[0] << wav_header.RIFF[1] << wav_header.RIFF[2] << wav_header.RIFF[3] <<  '\n';
	    cout << "WAVE header                :" << wav_header.WAVE[0] << wav_header.WAVE[1] << wav_header.WAVE[2] << wav_header.WAVE[3] << '\n';
	    cout << "FMT                        :" << wav_header.fmt[0] << wav_header.fmt[1] << wav_header.fmt[2] << wav_header.fmt[3] <<  '\n';
	    cout << "Data size                  :" << wav_header.chunk_size << '\n';
	    // Display the sampling Rate from the header
	    cout << "Sampling Rate              :" << wav_header.samples_per_sec <<  '\n';
	    cout << "Number of bits used        :" << wav_header.bits_per_sample <<  '\n';
	    cout << "Number of channels         :" << wav_header.num_channels <<  '\n';
	    cout << "Number of bytes per second :" << wav_header.bytes_per_sec <<  '\n';
	    cout << "Data length                :" << wav_header.sub_chunk_2_size <<  '\n';
	    cout << "Audio Format               :" << wav_header.audio_format <<  '\n';
	    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

	    cout << "Block align                :" << wav_header.block_align << '\n';
	    cout << "Data string                :" << wav_header.sub_chunk_2_ID[0] << wav_header.sub_chunk_2_ID[1] << wav_header.sub_chunk_2_ID[2] << wav_header.sub_chunk_2_ID[3] << '\n';


	    const size_t filelength = getFileSize(wav_file);
  		const size_t buffer_size = filelength - sizeof(wav_header);


  		file_to_return.header = wav_header;
  		file_to_return.data.push_back({});
  		file_to_return.data[0].resize(buffer_size);

	    const size_t data_bytes_read = fread(file_to_return.data[0].data(), sizeof(uint8_t), buffer_size / sizeof(uint8_t), wav_file);
	    if (data_bytes_read > 0)
	    {
	    	cout << "read " << data_bytes_read << "bytes" << '\n';
	    }

    }
    fclose(wav_file);
    return file_to_return;
}


Sound_Device::Sound_Device()
//:
{
	// Device = alcOpenDevice(nullptr);
	// if (Device)
	// {
	// 	Context = alcCreateContext(Device, nullptr);
	// 	alcMakeContextcurrent(Context);
	// }

	// g_bEAX = alIsExtensionPresent("EAX2.0");
	// alGetError();

	// alGenBuffers(NUM_BUFFERS, g_Buffers);

	// if ((error = alGetError()) != AL_NO_ERROR)
	// {
	// 	DisplayAlError("alGenBuffers: ", error);
	// }

}


// void alBufferData(ALuint buffer,
// 				  ALenum format,
// 				  const ALvoid *data,
// 				  ALsizei size,
// 				  ALsizei freq);

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
