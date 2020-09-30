/*#include "fakewalk.h"

#define GetPercentVal(val, percent) (val * (percent/100.f))

void FakeWalk::CreateMove(CUserCmd* cmd){

    if (!Settings::AntiAim::FakeWalk::enabled)
        return;
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
	if ( cmd->buttons & IN_ATTACK )
		return;
    if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeWalk::Key))
        return;

	if (ticks > 13){
		ticks = 0;
	}else {
		ticks++;
	}

	int maxTick = GetPercentVal(14, Settings::AntiAim::FakeWalk::Speed);
	bool canMove = !ticks || ticks > maxTick;

	if (canMove)
	{
		if (cmd->forwardmove)
			cmd->forwardmove = 0;
		if (cmd->sidemove)
			cmd->sidemove = 0;
	}	
   	CreateMove::sendPacket = !ticks;
}
*/
