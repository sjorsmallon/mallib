#ifndef INCLUDED_MSOUND_
#define INCLUDED_MSOUND_
// #include "../shared/shared.h"

namespace msound
{
    std::map<std::string, Wav_File> wav_files;
    std::map<std::string, uint32_t> sound_sources; 
    std::map<std::string, uint32_t> buffers;
    
    Wav_File load_wav_file(const char* filename); //Wav_File
    
    void register_wav_file(const char* filename); // Wav_File?

    void play_sound(const char* soundname);
    void play_sound(const std::string& soundname);
    // play_sound()

    // int get_file_size(FILE* inFile);
};


#endif
