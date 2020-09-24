#include "autostrafe.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/math.h"

static void LegitStrafe(C_BasePlayer* localplayer, CUserCmd* cmd)
{
	if (localplayer->GetFlags() & FL_ONGROUND)
		return;

	if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
		return;

	if (cmd->mousedx <= 1 && cmd->mousedx >= -1)
		return;

	switch (Settings::AutoStrafe::type)
	{
		case AutostrafeType::AS_FORWARDS:
			cmd->sidemove = cmd->mousedx < 0.f ? -250.f : 250.f;
			break;
		case AutostrafeType::AS_BACKWARDS:
			cmd->sidemove = cmd->mousedx < 0.f ? 250.f : -250.f;
			break;
		case AutostrafeType::AS_LEFTSIDEWAYS:
			cmd->forwardmove = cmd->mousedx < 0.f ? -250.f : 250.f;
			break;
		case AutostrafeType::AS_RIGHTSIDEWAYS:
			cmd->forwardmove = cmd->mousedx < 0.f ? 250.f : -250.f;
			break;
		default:
			break;
	}
}

float NormalizeYaw( const float& yaw ) {
	while ( yaw > 180.0f )
	return	yaw - 360.0f;
	while ( yaw < -180.0f )
	return	yaw + 360.0f;
}
static void DirectionalStrafe(C_BasePlayer* localplayer, CUserCmd* cmd){

if (localplayer->GetFlags() & FL_ONGROUND)
		return;


	float speed = localplayer->GetVelocity().Length();
	Vector velocity = localplayer->GetVelocity();
	float yawVelocity = RAD2DEG(atan2(velocity.y, velocity.x));
	float velocityDelta = NormalizeYaw(cmd->viewangles.y - yawVelocity);
	static float sideSpeed = cvar->FindVar("cl_sidespeed")->GetFloat();

	if (fabsf(cmd->mousedx > 2)) {

		cmd->sidemove = (cmd->mousedx < 0.f) ? -sideSpeed : sideSpeed;
		return;
	}

	if (cmd->buttons & IN_BACK)
		cmd->viewangles.y -= 180.f;
	else if (cmd->buttons & IN_MOVELEFT)
		cmd->viewangles.y -= 90.f;
	else if (cmd->buttons & IN_MOVERIGHT)
		cmd->viewangles.y += 90.f;

	if (!speed > 0.5f || speed == NAN || speed == INFINITY) {

		cmd->forwardmove = 450.f;
		return;
	}

	cmd->forwardmove = std::clamp(5850.f / speed, -450.f, 450.f);

	if ((cmd->forwardmove < -450.f || cmd->forwardmove > 450.f))
		cmd->forwardmove = 0.f;

	cmd->sidemove = (velocityDelta > 0.0f) ? -sideSpeed : sideSpeed;
	cmd->viewangles.y = NormalizeYaw(cmd->viewangles.y - velocityDelta);
}

static void RageStrafe(C_BasePlayer* localplayer, CUserCmd* cmd)
{

        const auto vel = localplayer->GetVelocity().Length2D();

        if (vel < 1.f)
                return;
        if (localplayer->GetFlags() & FL_ONGROUND)
                return;

        if (cmd->mousedx > 1 || cmd->mousedx < -1)
                cmd->sidemove = cmd->mousedx < 0.f ? -450.f : 450.f;
        else
        {
                cmd->forwardmove = std::clamp(10000.f / vel, -450.0f, 450.0f);
                cmd->sidemove = cmd->command_number % 2 == 0 ? -450.f : 450.f;
        }

}

void AutoStrafe::CreateMove(CUserCmd* cmd)
{
	if (!Settings::AutoStrafe::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	if (!localplayer->GetAlive())
		return;

	if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	switch (Settings::AutoStrafe::type)
	{
		case AutostrafeType::AS_FORWARDS:
			return;
		case AutostrafeType::AS_BACKWARDS:
			return;
		case AutostrafeType::AS_LEFTSIDEWAYS:
			return;
		case AutostrafeType::AS_RIGHTSIDEWAYS:
			LegitStrafe(localplayer, cmd);
			break;
		case AutostrafeType::AS_RAGE:
			RageStrafe(localplayer, cmd);
			break;
		case AutostrafeType::AS_DIRECTIONAL:
			DirectionalStrafe(localplayer, cmd);
			break;
	}
}
