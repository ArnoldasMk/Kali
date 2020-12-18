#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "Tickbase.h"
#include <climits>
#ifndef absol
        #define absol(x) x < 0 ? x*-1 : x
#endif
int ticksMax = 50;
int ticks_allowed = 0;
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
 const auto netchannel = GetLocalClient(-1)->m_NetChannel;
  int pakets = netchannel->m_nChokedPackets;

		if (cmd->buttons & IN_ATTACK && Settings::FakeLag::shiftshot){
                FakeLag::shift = 16;
		cmd->buttons &= IN_ATTACK;
			ticks_allowed = 0;
                //cmd->buttons &= ~IN_ATTACK;

		}else
		  FakeLag::shift = 0;
       	FakeLag::should_recharge = false;

        if (FakeLag::should_recharge)
        {
                ++ticks_allowed;
                CreateMove::sendPacket = true;

                cmd->tick_count = INT_MAX;
                cmd->forwardmove = 0.0f;
                cmd->sidemove = 0.0f;
                cmd->upmove = 0.0f;
                cmd->buttons &= ~IN_ATTACK;
                cmd->buttons &= ~IN_ATTACK2;

                if (ticks_allowed >= 14)
                {
	//		cvar->ConsoleDPrintf(XORSTR("we charged bois"));
                  //      FakeLag::should_recharge = false;
                       // *(bool*)(*frame_ptr - 0x1C) = true; 
                }

        }
		//if (ticks_allowed < 14)
		//FakeLag::should_recharge = true;
		//if (FakeLag::should_recharge)
		//	return;
	        if (cmd->buttons & IN_ATTACK)
	        {
                CreateMove::sendPacket = true;
   		C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon() );

		if (Settings::AntiAim::ChokeOnShot && activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_GRENADE)
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

		 if (FakeLag::ticks >= max_choke || ((*csGameRules)->IsValveDS() && pakets > 6)){
			CreateMove::sendPacket = true;
			FakeLag::ticks = -1;
		}else{
			CreateMove::sendPacket = false;
		}
	FakeLag::ticks++;
}

