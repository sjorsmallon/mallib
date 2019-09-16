#include "sound.h"


void sound::perform_setup()
{
	sound::soloud.init();
}

void sound::perform_shutdown()
{
	sound::soloud.deinit();
}