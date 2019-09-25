#include "sound.h"

SoLoud::Soloud& sound::soloud()
{
	static SoLoud::Soloud so_loud;
	return so_loud;
}

void sound::init_sound()
{
	sound::soloud().init();
}

void sound::perform_shutdown()
{
	sound::soloud().deinit();
}