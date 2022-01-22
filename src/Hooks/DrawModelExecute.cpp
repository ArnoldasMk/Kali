#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Features/chams.h"
#include "../Features/esp.h"

typedef void (*DrawModelExecuteFn) (void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);

void Hooks::DrawModelExecute(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot())
	{
		Chams::DrawModelExecute(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}

 	static matrix3x4_t BodyBoneMatrix[128];
        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if ((Settings::FakeLag::enabled && Settings::AnimMemes::enabled) || (Settings::FakeDuck::enabled && inputSystem->IsButtonDown(Settings::FakeDuck::key)) || (Settings::FakeLag::enabled && localplayer->GetVelocity().Length2D() < 2.0f)){
		if(!CreateMove::sendPacket && pInfo.entity_index == engine->GetLocalPlayer()){
			for (size_t i = 0; i < 128; i++)
			{
				pCustomBoneToWorld[i] = BodyBoneMatrix[i];
			}
		}else if ( pInfo.entity_index == engine->GetLocalPlayer() ){
			for (int i = 0; i < 128; i++){
				BodyBoneMatrix[i] = pCustomBoneToWorld[i];
			}
		}
	}
	modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);

	modelRender->ForcedMaterialOverride(nullptr);

	if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot())
	{
		ESP::DrawModelExecute();
	}
}
