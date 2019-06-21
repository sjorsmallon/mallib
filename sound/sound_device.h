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