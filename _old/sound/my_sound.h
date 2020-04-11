#ifndef INCLUDED_SOUND_
#define INCLUDED_SOUND_

// #include "../sound_device/sound_device.h"
#include <map>
#include <vector>
#include <stdint.h>

using uint32_t = Handle;

namespace sound
{
	inline static Sound_Device sound_device = {};
    inline static std::map<std::string, Wav_File> wav_files = {};
    inline static std::map<std::string, uint32_t> sound_sources = {};
    inline static std::map<std::string, uint32_t> buffers = {};
    inline static std::map<std::string, uint32_t> buffer_to_sources = {};
    inline static std::map<std::string, std::vector<uint32_t>> music_buffers = {};

 //    extern std::map<std::string, Wav_File> wav_files;
 //    extern std::map<std::string, uint32_t> sound_sources; // not for msound?
 //    extern std::map<std::string, uint32_t> buffers; // for msound?
 //    extern std::map<uint32_t, uint32_t> buffer_to_sources;
//     extern std::map<std::string, std::vector<uint32_t>> music_buffers;
    
    Sound_Device& sound_device();
    std::map<std::string, Wav_File> wav_files = {};
    std::map<std::string, uint32_t> sound_sources = {};

    Wav_File    load_wav_file(const char* filename); //Wav_File
    uint32_t    register_wav_file(const char* filename); // Wav_File?
    uint32_t    buffer_to_source(const uint32_t buffer);

    Handle      play_sound(const char* soundname);
    Handle      play_clocked(const char* soundname);
    Handle      play_music(const char* musicname); // play_background?
    
    void        stop_all();
    void        stop_audio_source(Handle handle); // ??

    void        pause_all();
    void        resume_all();

    float       get_global_volume();
    void        set_global_volume(float target_volume);
    // void        set_global_filter();
    void        fade_global_volume(float dt);


    // @Refactor: dt is in seconds here. change to ms?
    void        set_volume(Handle handle, float volume_t);
    void        set_pan(Handle handle, float pan);
    void        set_relative_play_speed(Handle Handle, float speed_t);
    void        set_pause(Handle handle, bool paused = false);
    void        set_protect_voice(Handle handle, Bool protected = true);

    void        fade_volume(Handle handle, float target_volume, float dt);

    void        schedule_pause(Handle handle, float dt);
    void        schedule_stop(Handle handle, float dt);

    void        oscillate_volume(Handle handle, float bottom_volume, float top_volume, float dt);
    Handle      create_voice_group();
    void        add_voice_to_group(Handle group_handle, Handle member_handle);
    void        destroy_voice_group();


};


#endif
