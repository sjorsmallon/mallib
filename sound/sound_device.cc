#include "sound_device.h"

Sound_Device::Sound_Device()
//:
{
  Device = alcOpenDevice(nullptr);
  if (Device)
  {
    Context = alcCreateContext(Device, nullptr);
    alcMakeContextCurrent(Context);
  }

  g_bEAX = alIsExtensionPresent("EAX2.0");
  alGetError(); // clear error code

  alGenBuffers(NUM_BUFFERS, g_Buffers);

  if ((error = alGetError()) != AL_NO_ERROR)
  {
    DisplayAlError("alGenBuffers : ", error);
  }

}


void Sound_Device::load_wav_file(const char* filename)
{
    // format, data, size, freq, loop ?
    // g_BUffers & num_buffers.
    // error.
    loadWAVFile(filename, &format,&data,&size,&freq, &loop);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        DisplayALError("alutLoadWAVFile error:",error);
        alDeleteBuffers(NUM_BUFFERS, g_Buffers);
        return;
    }

    alBufferData(g_Buffers[0], format, data, size, freq);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        DisplayALError("alutLoadWAVFile error:",error);
        alDeleteBuffers(NUM_BUFFERS, g_Buffers);
        return;
    }
}



void Sound_Device::data_to_buffer()
{

}

void Sound_Device::play_sound(const char* filename)
{ 
    
}

void Sound_Device::play_music(const char* filename)
{

}