#include "fakeduck.h"

#include "../Hooks/hooks.h"
#include "../interfaces.h"
#include "../settings.h"

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
                cmd->buttons |= IN_BULLRUSH;

                if (choked <= 6){
                        cmd->buttons &= ~IN_DUCK;
                }else{
                        cmd->buttons |= IN_DUCK;
               } if (choked < 14){
			choked++;
                        CreateMove::sendPacket = false;   // choke
		}
                else{
                       CreateMove::sendPacket = true;    // send packet
		       choked = 0;
		}
if (input->m_fCameraInThirdPerson)
localplayer->GetAnimState()->duckProgress = 1.0; //memes

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
