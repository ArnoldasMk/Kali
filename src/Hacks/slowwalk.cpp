#include "slowwalk.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "../../Utils/xorstring.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../SDK/vector.h"

void FakeWalk::CreateMove(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    if (!Settings::AntiAim::SlowWalk::enabled || !Settings::AntiAim::SlowWalk::key)
        return;
  if (!inputSystem->IsButtonDown(Settings::AntiAim::SlowWalk::key))
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
//     float amount = 0.0034f * Settings::AntiAim::SlowWalk::Speed; // options.misc.slow_walk_amount has 100 max value

//     Vector velocity = localplayer->GetVelocity();
//     QAngle direction;

//     //Math::get( ).vector_angles(velocity, direction);
// Math::AngleVectors(direction, velocity);
//     float speed = localplayer->GetVelocity().Length2D();
//     direction.y = cmd->viewangles.y - direction.y;

//     Vector forward;

//     //Math::get().angle_vectors(direction, forward);
// Math::AngleVectors(direction,forward);
//     Vector source = forward * -speed;

//     if (speed >= (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() * amount))
//     {
//         cmd->forwardmove = source.x;
//         cmd->sidemove = source.y;

//     }
        if (activeWeapon) {


                        float speed = 0.1f;
                                float max_speed = activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed();
                                float ratio = max_speed / 250.0f;
                                speed *= ratio;
                        


                        cmd->forwardmove *= speed;
                        cmd->sidemove *= speed;
        }
}
