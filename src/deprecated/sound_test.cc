

	// auto Device = alcOpenDevice(nullptr);
	// if (Device)
	// {
	// 	auto Context = alcCreateContext(Device, nullptr);
	// 	alcMakeContextCurrent(Context);
	// }

	// ALenum error;
	// const int num_buffers = 1;
	// std::vector<ALuint> buffers(1);

	// alGenBuffers(num_buffers, buffers.data());

	// if ((error = alGetError()) != AL_NO_ERROR)
	// {
	// 	printf("errortjeee");
	// 	// DisplayALError("alGenBuffers: ", error);
	// }

	// alBufferData(buffers[0],  AL_FORMAT_MONO8, wav_file.data[0].data(), wav_file.data[0].size() * sizeof(uint8_t), wav_file.header.samples_per_sec);
	// if ((error = alGetError()) != AL_NO_ERROR)
	// {
	// 	// DisplayAlError("alutLoadWavFILE error:", error)
	// 	alDeleteBuffers(num_buffers, buffers.data());
	// 	exit(1);
	// }

	
	// cerr << "no error in alBufferData";
	// ALuint sound_source;
	// alGenSources(1, &sound_source);
	// alSourcei(sound_source, AL_BUFFER, buffers[0]);
	// // alSourcei(sound_source, AL_LOOPING, AL_TRUE);

	// alSourcePlay(sound_source);

	// while(true)
	// {

	// }

	// alDeleteSources(1, &sourceid);
 //    alDeleteBuffers(1, &bufferid);
 //    alcDestroyContext(context);
 //    alcCloseDevice(device);
