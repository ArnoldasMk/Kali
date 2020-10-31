#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "Tickbase.h"
#ifndef absol
        #define absol(x) x < 0 ? x*-1 : x
#endif
int ticksMax = 50;

bool CheckPeaking(CUserCmd* cmd){
        float forMove = absol(cmd->forwardmove);
        float sideMove = absol(cmd->sidemove);
        if (sideMove > forMove) {
                C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
                if (!localplayer || !localplayer->GetAlive()){
                        return false;
                }

                if ( localplayer->GetVelocity().Length2D() > 100.f )
                        return true;
        }       
        return false;
}
void LagSpike(CUserCmd* cmd, int lagTick){
        if (!CheckPeaking){
                ticksMax = 16;
                return;
        }

        ticksMax = 25;
        FakeLag::ticks = 0;


}
void FakeLag::CreateMove(CUserCmd* cmd)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if (Settings::FakeLag::adaptive)
	{
		int packetsToChoke;
		if (localplayer->GetVelocity().Length() > 0.f)
		{
			packetsToChoke = (int)((64.f / globalVars->interval_per_tick) / localplayer->GetVelocity().Length()) + 1;
			if (packetsToChoke >= 15)
				packetsToChoke = 14;
			if (packetsToChoke < Settings::FakeLag::value)
				packetsToChoke = Settings::FakeLag::value;
		}
		else
			packetsToChoke = 0;
		
		CreateMove::sendPacket = FakeLag::ticks < (16 - packetsToChoke);
	}
	else{
	if (FakeLag::ticks >= Settings::FakeLag::value){
			CreateMove::sendPacket = true;
			FakeLag::ticks = -1;
		}else{
			CreateMove::sendPacket = false;
		}
	}

	FakeLag::ticks++;
}

