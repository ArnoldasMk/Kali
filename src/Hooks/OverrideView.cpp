#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Features/fovchanger.h"
#include "../Features/thirdperson.h"
#include "../Features/grenadeprediction.h"
#include "../Features/fakeduck.h"
#include "../Features/antiaim.h"

float OverrideView::currentFOV = 90.0f;

typedef void (*OverrideViewFn) (void*, CViewSetup*);

void Hooks::OverrideView(void* thisptr, CViewSetup* pSetup)
{
	if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot())
	{
		GrenadePrediction::OverrideView(pSetup);
		FOVChanger::OverrideView(pSetup);
		ThirdPerson::OverrideView(pSetup);
		FakeDuck::OverrideView(pSetup);
		AntiAim::OverrideView(pSetup);
	}

	OverrideView::currentFOV = pSetup->fov;

	clientModeVMT->GetOriginalMethod<OverrideViewFn>(19)(thisptr, pSetup);
}
