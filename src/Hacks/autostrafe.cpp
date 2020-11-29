#include "autostrafe.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/math.h"
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
#define M_RADPI 57.295779513082f
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
       void vector_angles(const Vector& forward, Vector& angles)
        {
                Vector view;

                if (!forward[0] && !forward[1])
                {
                        view[0] = 0.0f;
                        view[1] = 0.0f;
                }
                else
                {
                        view[1] = atan2(forward[1], forward[0]) * 180.0f / M_PI;

                        if (view[1] < 0.0f)
                                view[1] += 360.0f;

                        view[2] = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
                        view[0] = atan2(forward[2], view[2]) * 180.0f / M_PI;
                }

                angles[0] = -view[0];
                angles[1] = view[1];
                angles[2] = 0.f;
        }

float NormalizeYaw( const float& yaw ) {
	while ( yaw > 180.0f )
	return	yaw - 360.0f;
	while ( yaw < -180.0f )
	return	yaw + 360.0f;
}

void anglevectors(const Vector& angles, Vector& forward)
{
          float sp, sy, cp, cy;

          sy = sin(DEG2RAD(angles[1]));
          cy = cos(DEG2RAD(angles[1]));

          sp = sin(DEG2RAD(angles[0]));
          cp = cos(DEG2RAD(angles[0]));

          forward.x = cp * cy;
          forward.y = cp * sy;
          forward.z = -sp;
}

static void DirectionalStrafe(C_BasePlayer* localplayer, CUserCmd* cmd){

        static auto cl_sidespeed = cvar->FindVar("cl_sidespeed");
        auto side_speed = cl_sidespeed->GetFloat();
                static auto old_yaw = 0.0f;
        if (localplayer->GetFlags() & FL_ONGROUND)
                return;
	if (!(cmd->buttons & IN_JUMP))
		return;
                auto get_velocity_degree = [](float velocity)
                {
                        auto tmp = RAD2DEG(atan(30.0f / velocity));

                        if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
                                return 90.0f;

                        else if (tmp < 0.0f)
                                return 0.0f;
                        else
                                return tmp;
                };

                if (localplayer->GetMoveType() == MOVETYPE_WALK)
                        return;

                auto velocity = localplayer->GetVelocity();
                velocity.z = 0.0f;

                static auto flip = false;
                flip = !flip;

                auto turn_direction_modifier = flip ? 1.0f : -1.0f;
                auto forwardmove = cmd->forwardmove;
                auto sidemove = cmd->sidemove;
                if (velocity.Length2D() < 5.0f && !forwardmove && !sidemove)
                        return;
                auto viewangles = cmd->viewangles;
                if (forwardmove || sidemove)
                {
                        cmd->forwardmove = 0.0f;
                        cmd->sidemove = 0.0f;

                        auto turn_angle = atan2(-sidemove, forwardmove);
                        viewangles.y += turn_angle * M_RADPI;
                }
                else if (forwardmove) //-V550
                        cmd->forwardmove = 0.0f;
                auto strafe_angle = RAD2DEG(atan(15.0f / velocity.Length2D()));
                if (strafe_angle > 90.0f)
                        strafe_angle = 90.0f;
                else if (strafe_angle < 0.0f)
                        strafe_angle = 0.0f;

               auto temp = Vector(0.0f, viewangles.y - old_yaw, 0.0f);
                temp.y = NormalizeYaw(temp.y);
                auto yaw_delta = temp.y;
                old_yaw = viewangles.y;
                auto abs_yaw_delta = fabs(yaw_delta);
                if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.0f)
                {
                        Vector velocity_angles;
                        anglevectors(velocity, velocity_angles);

                        temp = Vector(0.0f, viewangles.y - velocity_angles.y, 0.0f);
                        temp.y = NormalizeYaw(temp.y);

                        auto velocityangle_yawdelta = temp.y;
                        auto velocity_degree = get_velocity_degree(velocity.Length2D());

                        if (velocityangle_yawdelta <= velocity_degree || velocity.Length2D() <= 15.0f)
                        {
                                if (-velocity_degree <= velocityangle_yawdelta || velocity.Length2D() <= 15.0f)
                                {
                                        viewangles.y += strafe_angle * turn_direction_modifier;
                                        cmd->sidemove = side_speed * turn_direction_modifier;
                                }
                                else
                                {
                                        viewangles.y = velocity_angles.y - velocity_degree;
                                        cmd->sidemove = side_speed;
                                }
                        }
                        else
                        {
                                viewangles.y = velocity_angles.y + velocity_degree;
                                cmd->sidemove = -side_speed;
                        }
                }
                else if (yaw_delta > 0.0f)
                        cmd->sidemove = -side_speed;
                else if (yaw_delta < 0.0f)
                        cmd->sidemove = side_speed;

                auto move = Vector(cmd->forwardmove, cmd->sidemove, 0.0f);
                auto speed = move.Length();
                Vector angles_move;
                anglevectors(move, angles_move);

                auto normalized_x = fmod(cmd->viewangles.x + 180.0f, 360.0f) - 180.0f;
                auto normalized_y = fmod(cmd->viewangles.y + 180.0f, 360.0f) - 180.0f;

                auto yaw = DEG2RAD(normalized_y - viewangles.y + angles_move.y);

                if (normalized_x >= 90.0f || normalized_x <= -90.0f || cmd->viewangles.x >= 90.0f && cmd->viewangles.x <= 200.0f || cmd->viewangles.x <= -90.0f && cmd->viewangles.x <= 200.0f) //-V648
                        cmd->forwardmove = -cos(yaw) * speed;
                else
                        cmd->forwardmove = cos(yaw) * speed;

                cmd->sidemove = sin(yaw) * speed;

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
