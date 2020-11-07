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
    QAngle ViewAngle;
        engine->GetViewAngles(ViewAngle);
if ( Settings::AntiAim::SlowWalk::mode == SlowMode::SAFETY){
    static Vector oldOrigin = localplayer->GetAbsOrigin( );
        Vector velocity = ( localplayer->GetVecOrigin( )-oldOrigin )    
                                                        * (1.f/globalVars->interval_per_tick);
        oldOrigin = localplayer->GetAbsOrigin( );
        float speed  = velocity.Length( );

    if(speed > Settings::AntiAim::SlowWalk::Speed )
        {
                cmd->forwardmove = 0;
                cmd->sidemove = 0;
                CreateMove::sendPacket = false;
        }
        else {
                CreateMove::sendPacket = true;
        }
}else {
   if (activeWeapon) {


                        float speed = 0.1f;
                                float max_speed = activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed();
                                float ratio = max_speed / 250.0f;
                                speed *= ratio;


                        cmd->forwardmove *= speed;
                        cmd->sidemove *= speed;
        }
}

}

