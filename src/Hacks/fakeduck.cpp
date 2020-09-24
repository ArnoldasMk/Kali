#include "fakeduck.h"

#include "../Hooks/hooks.h"
#include "../interfaces.h"
#include "../settings.h"

// Find it from nimbus there is no point to wast your time if that is all ready available
static bool FirstDuck = false;
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
  static bool counter = false;
            static int counters = 0;
            if (counters == 9)
            {
                counters = 0;
                counter = !counter;
            }
            counters++;
            if (counter)
            {
                cmd->buttons |= IN_DUCK;
                CreateMove::sendPacket = true;
            }
            else
            {
                CreateMove::sendPacket = false;
                cmd->buttons &= ~IN_DUCK;
            }
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
