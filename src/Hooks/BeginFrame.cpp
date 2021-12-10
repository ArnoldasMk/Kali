#include "hooks.h"

#include "../Utils/skins.h"
#include "../interfaces.h"

#include "../Features/clantagchanger.h"
#include "../Features/namechanger.h"
#include "../Features/namestealer.h"
#include "../Features/spammer.h"
#include "../Features/radar.h"
#include "../Features/disablepostprocessing.h"

typedef void (*BeginFrameFn) (void*, float);

void Hooks::BeginFrame(void* thisptr, float frameTime)
{
	ClanTagChanger::BeginFrame(frameTime);
	NameChanger::BeginFrame(frameTime);
	NameStealer::BeginFrame(frameTime);
	Spammer::BeginFrame(frameTime);
	Radar::BeginFrame();
	DisablePostProcessing::BeginFrame();

	if (!engine->IsInGame())
		CreateMove::sendPacket = true;

	Skins::Localize();

	return materialVMT->GetOriginalMethod<BeginFrameFn>(42)(thisptr, frameTime);
}
