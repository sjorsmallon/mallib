#ifndef INCLUDED_MSOUND_
#define INCLUDED_MSOUND_

#include "../sound_device/sound_device.h"
#include <map>
#include <vector>

struct Playing_Sound
{
    Vec2 current_volume; // left, right
    Vec2 dCurrent_volume;
    Vec2 target_volume;

    float dSample; // current sample that is playing?
    Sound_ID id;
    float samples_played;
    // playing_sound* next_sound;
}

struct Audio_State
{
    Memory_Arena perm_arena;
    //Playing_Sound* first_playing_sound; // linked list of playing sounds
    Playing_Sound;
}

namespace msound
{


	extern Sound_Device sound_device;
    extern std::map<std::string, Wav_File> wav_files;
    extern std::map<std::string, uint32_t> sound_sources; // not for msound?
    extern std::map<std::string, uint32_t> buffers; // for msound?
    extern std::map<uint32_t, uint32_t> buffer_to_sources;
	extern std::map<std::string, std::vector<uint32_t>> music_buffers;
    

    Wav_File load_wav_file(const char* filename); //Wav_File
    uint32_t register_wav_file(const char* filename); // Wav_File?
    uint32_t buffer_to_source(const uint32_t buffer);



    void     play_sound(const char* soundname);
    void     play_music(const char* musicname);
    void	 pause_music();
    void	 stop_music();

};


#endif
