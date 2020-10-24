#include "animfix.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"
#include "../Utils/draw.h"
#include "../SDK/vector.h"

void AnimFix::FrameStageNotify(ClientFrameStage_t stage)
{
    if (!engine->IsInGame())
        return;

        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

   if (!localplayer || !localplayer->GetAlive())
        return;
static auto backup_abs = localplayer->GetAnimState()->goalFeetYaw;
if (!input->m_fCameraInThirdPerson){
localplayer->ClientAnimations(true);
localplayer->updateClientAnimation();
localplayer->ClientAnimations(false);
	return;
}
    static int old_tick = 0;
    if (old_tick != globalVars->tickcount)
    {
	old_tick = globalVars->tickcount;
	localplayer->ClientAnimations(true);
	localplayer->updateClientAnimation();
	localplayer->ClientAnimations(false);
	if (CreateMove::sendPacket)
	{
	backup_abs = localplayer->GetAnimState()->goalFeetYaw;
	}
   }
  localplayer->GetAnimState()->goalFeetYaw = 0.f;
//  localplayer->SetAbsOrigin(Vector(0, backup_abs,0 ));

}
