#include "bhop.h"

#include "../settings.h"
#include "../interfaces.h"

void BHop::CreateMove(CUserCmd* cmd)
{
	if (!Settings::BHop::enabled)
		return;

	static bool bLastJumped = false;
	static bool bShouldFake = false;
	static int bActualHop = 0;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
		return;


	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		cmd->buttons |= IN_JUMP;
	}
	else if (cmd->buttons & IN_JUMP)
	{
		if (localplayer->GetFlags() & FL_ONGROUND)
		{
			bActualHop++;
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			if (Settings::BHop::Chance::enabled &&
				Settings::BHop::Hops::enabledMin &&
				(bActualHop > Settings::BHop::Hops::Min) &&
				(rand()%100>Settings::BHop::Chance::value))
				return;

			if (Settings::BHop::Chance::enabled &&
				!Settings::BHop::Hops::enabledMin &&
				(rand()%100 > Settings::BHop::Chance::value))
				return;

			if (Settings::BHop::Hops::enabledMin &&
				!Settings::BHop::Chance::enabled &&
				(bActualHop > Settings::BHop::Hops::Min))
				return;

			if (Settings::BHop::Hops::enabledMax &&
				(bActualHop>Settings::BHop::Hops::Max))
				return;

			cmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bActualHop = 0;
		bLastJumped = false;
		bShouldFake = false;
	}
}
