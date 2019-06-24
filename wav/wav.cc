#include <iostream>
#include <string>
#include <fstream>


using std::cin;
using std::cout;
using std::fstream;
using std::string;

#include <cstdint>
struct Wav_Header
{
    // RIFF Chunk Descriptor
    uint8_t         RIFF[4];           // RIFF Header Magic header
    uint32_t        chunk_size;        // RIFF Chunk Size
    uint8_t         WAVE[4];           // WAVE Header
    // "fmt" sub-chunk
    uint8_t         fmt[4];            // FMT header
    uint32_t        sub_chunk_1_size;  // Size of the fmt chunk
    uint16_t        audio_format;      // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        num_of_chan;       // Number of channels 1=Mono 2=Sterio
    uint32_t        samples_per_sec;   // Sampling Frequency in Hz
    uint32_t        bytes_per_sec;     // bytes per second
    uint16_t        block_align;       // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bits_per_sample;   // Number of bits per sample
    // "data" sub-chunk
    uint8_t         sub_chunk_2_ID[4]; // "data"  string
    uint32_t        sub_chunk_2_size;  // Sampled data length
};

// Function prototypes
int getFileSize(FILE* inFile);

int main()
{
    Wav_Header wav_header;
    int header_size = sizeof(wav_header);
    int filelength = 0;

    const char* filePath = "chicken.wav";

    FILE* wav_file = fopen(filePath, "r");
    if (wav_file == nullptr)
    {
        fprintf(stderr, "Unable to open wave file: %s\n", filePath);
        return 1;
    }

    //Read the header
    size_t bytes_read = fread(&wav_header, 1, header_size, wav_file);
    cout << "Header Read " << bytes_read << " bytes." << '\n';
    if (bytes_read > 0)
    {
        //Read the data
        uint16_t bytes_per_sample = wav_header.bits_per_sample / 8;      //Number     of bytes per sample
        uint64_t num_samples = wav_header.chunk_size / bytes_per_sample; //How many samples are in the wav file?
        static const uint16_t BUFFER_SIZE = 4096;
        int8_t* buffer = new int8_t[BUFFER_SIZE];

        while ((bytes_read = fread(buffer, sizeof buffer[0], BUFFER_SIZE / (sizeof buffer[0]), wav_file)) > 0)
        {
            // plot the data while reading?
            cout << "Read " << bytes_read << " bytes." << '\n';
        }
        delete [] buffer;
        buffer = nullptr;
        filelength = getFileSize(wav_file);

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
    }
    fclose(wav_file);
    return 0;
}

// find the file size
int getFileSize(FILE* inFile)
{
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}