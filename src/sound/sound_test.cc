#include "sound.h"




using uint32_t = Handle;



int main()
{
	// SoLoud::Soloud soloud;
	SoLoud::Wav sample;

	sound::soloud.init();
	int handle = sample.load("chicken.wav");
	sound::soloud.play(sample);

	while(sound::soloud.getActiveVoiceCount() > 0)
	{
		SoLoud::Thread::sleep(100);
	}

	sound::soloud.deinit();

}
