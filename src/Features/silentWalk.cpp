#include "silentWalk.h"
#include "../interfaces.h"
#include "../settings.h"
#include "../ImGUI/imgui.h"

float Magnitude(Vector a)
{
	return sqrt((a.x * a.x) + (a.y * a.y));
}
Vector Normalize(Vector value)
{
	float num = Magnitude(value);
	if (num != 0.f)
		return value / num;
	return Vector(0.f, 0.f, 0.f);
}
Vector ClampMagnitude(Vector vector, float maxLength)
{
	if (Magnitude(vector) > maxLength)
		return Vector(Normalize(vector).x * maxLength, Normalize(vector).y * maxLength, 0);
	return vector;
}
void SilentWalk::CreateMove(CUserCmd* cmd)
{
  C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

	Vector moveDir = Vector(0.f, 0.f, 0.f);
	float maxSpeed = 133.f; //can be 134 but sometimes I make a sound, 130 works perfectly
	int movetype = localplayer->GetMoveType();
	bool InAir = !(localplayer->GetFlags() & FL_ONGROUND);
	if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || cmd->buttons & IN_DUCK || InAir || !(cmd->buttons & IN_SPEED)/* When holding knife or bomb*/) //IN_WALK doesnt work
		return;
if (!(Settings::SilentWalk::enabled))
		return;
	moveDir.x = cmd->sidemove;
	moveDir.y = cmd->forwardmove;
	moveDir = ClampMagnitude(moveDir, maxSpeed);
	cmd->sidemove = moveDir.x;
	cmd->forwardmove = moveDir.y;
	if (!(localplayer->GetVelocity().Length2D() > maxSpeed + 1))
		cmd->buttons &= ~IN_SPEED;
}
