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

#define GetPercentVal(val, percent) (val * (percent/100.f))

void FakeWalk::CreateMove(CUserCmd* cmd){

    if (!Settings::AntiAim::SlowWalk::enabled)
        return;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

    if (!inputSystem->IsButtonDown(Settings::AntiAim::SlowWalk::key))
        return;

    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;

    QAngle ViewAngle;
	engine->GetViewAngles(ViewAngle);

    static Vector oldOrigin = localplayer->GetAbsOrigin( );
	Vector velocity = ( localplayer->GetVecOrigin( )-oldOrigin ) 	
							* (1.f/globalVars->interval_per_tick);
	oldOrigin = localplayer->GetAbsOrigin( );
	float speed  = velocity.Length( );

    if(speed > Settings::AntiAim::SlowWalk::Speed )
	{
		QAngle dir;
		Math::VectorAngles(velocity, dir);
		dir.y = ViewAngle.y - dir.x;
		Vector NewMove = Vector(0);
		Math::AngleVectors(dir, NewMove);
		auto max = std::max(oldForward, oldSideMove);
		auto mult = 450.f/max;
		NewMove *= -mult;
		cmd->forwardmove =  NewMove.x;
		cmd->sidemove =  NewMove.y;
	}
}
