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

void msound::load_wav_file(const char* filename)
{
	Wav_Header wav_header;
	int header_size = sizeof(wav_header);
	int filelength = 0;

	const char* filePath = "chicken.wav";

	FILE* wav_file = fopen(filePath, "r");
	if (wav_file == nullptr)
	{
	    fprintf(stderr, "Unable to open wav file: %s\n", filePath);
	    return;
	}

	//Read the header
	size_t bytes_read = fread(&wav_header, 1, header_size, wav_file);
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
	    cout << "Number of channels         :" << wav_header.num_of_chan <<  '\n';
	    cout << "Number of bytes per second :" << wav_header.bytes_per_sec <<  '\n';
	    cout << "Data length                :" << wav_header.sub_chunk_2_size <<  '\n';
	    cout << "Audio Format               :" << wav_header.audio_format <<  '\n';
	    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

	    cout << "Block align                :" << wav_header.block_align << '\n';
	    cout << "Data string                :" << wav_header.sub_chunk_2_ID[0] << wav_header.sub_chunk_2_ID[1] << wav_header.sub_chunk_2_ID[2] << wav_header.sub_chunk_2_ID[3] << '\n';

	    static const uint16_t BUFFER_SIZE = 4096;
	    int8_t* buffer = new int8_t[BUFFER_SIZE];
	    filelength = getFileSize(wav_file);

	    const int buffer_size = filelength - sizeof(wav_header);
	    cout  << "buffer size: " << buffer_size; 
	    /* mine
	    const int buffer_size = wav_header.sub_chunk_2_size;
	    int8_t* buffer = new int8_t[buffer_size];
	    filelength -= header_size;
	    fread(buffer,sizeof buffer[0], buffer_size, wav_file)
		*/

	    while ((bytes_read = fread(buffer, sizeof buffer[0], BUFFER_SIZE / (sizeof buffer[0]), wav_file)) > 0)
	    {
	        // plot the data while reading?
	        cout << "Read " << bytes_read << " bytes." << '\n';
	    }


	    delete [] buffer;
	    buffer = nullptr;

    }
    fclose(wav_file);
    return;
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
