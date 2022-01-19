#include "fakeduck.h"

#include "../Hooks/hooks.h"
#include "../interfaces.h"
#include "../settings.h"
#include "antiaim.h"
#include "valvedscheck.h"
#include "../../Utils/xorstring.h"

static bool firstDuck = false;
int choked;

void FakeDuck::CreateMove(CUserCmd *cmd)
{
	if (!Settings::FakeDuck::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
		return;

	if (!inputSystem->IsButtonDown(Settings::FakeDuck::key)) {
		firstDuck = false;
		return;
	} else {
		firstDuck = true;
	}

	if (firstDuck) {
		int amount;
        cmd->buttons |= IN_BULLRUSH;
		
		if ((*csGameRules)->IsValveDS())
		    amount = 7;
		else
		    amount = 14;
		if (choked <= amount / 2) {
            cmd->buttons &= ~IN_DUCK;
        } else {
			cmd->buttons |= IN_DUCK;
		}

		if (choked < amount) {
			choked++;
	        CreateMove::sendPacket = false;   // choke
		} else {
			CreateMove::sendPacket = true;    // send packet
			choked = 0;
		}
	}
}

void FakeDuck::OverrideView(CViewSetup *pSetup)
{
	if (!Settings::FakeDuck::enabled)
		return;

	if (!inputSystem->IsButtonDown(Settings::FakeDuck::key))
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	pSetup->origin.z = localplayer->GetAbsOrigin().z + 64.0f;
}
