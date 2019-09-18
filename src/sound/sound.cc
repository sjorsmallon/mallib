#include "sound.h"

SoLoud::Soloud& sound::soloud()
{
	static SoLoud::Soloud so_loud;
	return so_loud;
}

void sound::perform_setup()
{
	sound::soloud().init();
}

void sound::perform_shutdown()
{
	sound::soloud().deinit();
}