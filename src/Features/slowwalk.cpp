#include "slowwalk.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../SDK/vector.h"
#include "../Hooks/hooks.h"
#include "../Utils/entity.h"

void SlowWalk::CreateMove(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    if (!Settings::SlowWalk::enabled || !Settings::SlowWalk::key)
        return;
    if (!inputSystem->IsButtonDown(Settings::SlowWalk::key))
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    QAngle ViewAngle;
    engine->GetViewAngles(ViewAngle);

    if (Settings::SlowWalk::mode == SlowMode::SAFETY) {
        static Vector oldOrigin = localplayer->GetAbsOrigin();
        Vector velocity = (localplayer->GetVecOrigin() - oldOrigin) * (1.f / globalVars->interval_per_tick);
        oldOrigin = localplayer->GetAbsOrigin();
        float speed  = velocity.Length();

        if (speed > Settings::SlowWalk::speed) {
                cmd->forwardmove = 0;
                cmd->sidemove = 0;
                CreateMove::sendPacket = false;
        } else
                CreateMove::sendPacket = true;
    } else {
        if (activeWeapon) {
            float speed = 0.1f * (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 250.f);

            cmd->forwardmove *= speed;
            cmd->sidemove *= speed;
        }
    }
}

