#include "sound.h"

int main()
{
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
