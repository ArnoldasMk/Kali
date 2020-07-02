#include "animfix.h"

void AnimFix::FrameStageNotify(ClientFrameStage_t stage)
{
    if (!engine->IsInGame())
        return;
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer)
        return;

    CCSGOAnimState* animation = localplayer->GetAnimState();

    if (!animation)
        return;

    if (stage == ClientFrameStage_t::FRAME_RENDER_START)
    {
        float old_curtime = globalVars->curtime;
        float old_frametime = globalVars->frametime;
         
    }
}