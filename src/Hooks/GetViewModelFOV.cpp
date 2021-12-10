#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Features/fovchanger.h"

typedef float (*GetViewModelFOVFn) (void*);

float Hooks::GetViewModelFOV(void* thisptr)
{
	float fov = clientModeVMT->GetOriginalMethod<GetViewModelFOVFn>(36)(thisptr);

	if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot())
		FOVChanger::GetViewModelFOV(fov);

	return fov;
}