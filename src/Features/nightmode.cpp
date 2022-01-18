#include "nightmode.h"
#include "../settings.h"

// from gamesneeze :)
// *called in esp.cpp, line 2473
void Nightmode::onTonemap(CEnvTonemapController* tonemapController)
{
	*tonemapController->getUseExposureMin() = Settings::Nightmode::enabled;
	*tonemapController->getUseExposureMax() = Settings::Nightmode::enabled;
	*tonemapController->getExposureMin() = 0.5f - (Settings::Nightmode::value / 201.f);
	*tonemapController->getExposureMax() = 0.5f - (Settings::Nightmode::value / 201.f);
}