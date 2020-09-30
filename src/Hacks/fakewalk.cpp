#include "fakewalk.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"


void FakeeWalk::CreateMove(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

    static int choked = 0;

    if (Settings::FakeWalk::enabled)
    {
        if (!inputSystem->IsButtonDown(Settings::FakeWalk::key))
            return;

        choked = choked > 13 ? 0 : choked + 1;
        cmd->forwardmove = choked < 1 || choked > 8 ? 0 : cmd->forwardmove;
        cmd->sidemove = choked < 1 || choked > 8 ? 0 : cmd->sidemove;
        CreateMove::sendPacket = choked < 1;
    }
}






