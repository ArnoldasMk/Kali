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
	if (!Settings::FakeLag::enabled)
		return;
		int velocity2d = localplayer->GetVelocity().Length2D();
                int max_choke;
	        if (cmd->buttons & IN_ATTACK)
	        {
                CreateMove::sendPacket = true;
		if (Settings::AntiAim::ChokeOnShot)
		max_choke = 25;
    		}

		if (Settings::FakeLag::adaptive){
		if (velocity2d >= 5.0f)
                {
                        auto dynamic_factor = std::ceil(64.0f / (velocity2d * globalVars->interval_per_tick));

                        if (dynamic_factor > 16)
                                dynamic_factor = Settings::FakeLag::value;

                        max_choke = dynamic_factor;
                }
                else
                        max_choke = Settings::FakeLag::value;
		}else if ( !(cmd->buttons & IN_ATTACK) || !(Settings::AntiAim::ChokeOnShot) ) max_choke = Settings::FakeLag::value;
		if ((*csGameRules)->IsValveDS()){
		if (max_choke >= 7)
			max_choke = 7;
		}
		 if (FakeLag::ticks >= max_choke){
			CreateMove::sendPacket = true;
			FakeLag::ticks = -1;
		}else{
			CreateMove::sendPacket = false;
		}
	FakeLag::ticks++;
}

