#include "fakeduck.h"

#include "../Hooks/hooks.h"
#include "../interfaces.h"
#include "../settings.h"
#include "antiaim.h"
#include "valvedscheck.h"
#include "../../Utils/xorstring.h"

static bool FirstDuck = false;
int choked;
void FakeDuck::CreateMove(CUserCmd *cmd)
{
	if (!Settings::AntiAim::FakeDuck::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
		return;

	if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeDuck::fakeDuckKey))
	{
		FirstDuck = false;
		return;
	}else {
		FirstDuck = true;
		}

if (FirstDuck){
		int amount;
                cmd->buttons |= IN_BULLRUSH;
		if ((*csGameRules)->IsValveDS())
		    amount = 7;
		else
		    amount = 14;
                if (choked <= amount / 2){
                        cmd->buttons &= ~IN_DUCK;
			if (choked > (amount / 3) + 1 )
				localplayer->GetAnimState()->duckProgress = 0.f;
			else
				localplayer->GetAnimState()->duckProgress = 1.0;
                }else{
                        cmd->buttons |= IN_DUCK;
			localplayer->GetAnimState()->duckProgress = 1.0;
               } if (choked < amount){
			choked++;
                        CreateMove::sendPacket = false;   // choke
		}
                else{
                       CreateMove::sendPacket = true;    // send packet
		       choked = 0;
		}
if (input->m_fCameraInThirdPerson && Settings::AnimMemes::enabled)
                                localplayer->GetAnimState()->duckProgress = AntiAim::realDuck;

//localplayer->GetAnimState()->duckProgress = 1.0; //memes

}
}

void FakeDuck::OverrideView(CViewSetup *pSetup)
{
	if (!Settings::AntiAim::FakeDuck::enabled)
		return;

	if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeDuck::fakeDuckKey))
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	pSetup->origin.z = localplayer->GetAbsOrigin().z + 64.0f;
}
