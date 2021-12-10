#include "faststop.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/math.h"
/*
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

void faststop::CreateMove(CUserCmd* cmd)
{
        if (!Settings::faststop::enabled)
                return;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

        if (!(localplayer->GetFlags() & FL_ONGROUND))
                return;

        if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
                return;

                auto velocity = localplayer->GetVelocity();

                if (velocity.Length2D() > 20.0f)
                {
                        Vector direction;
                        Vector real_view;

                        vector_angles(velocity, direction);
                        engine->GetViewAngles(real_view);

                        direction.y = real_view.y - direction.y;

                        Vector forward;
                        angle_vectors(direction, forward);

			static auto cl_sidespeed = cvar->FindVar("cl_sidespeed");
			static auto cl_forwardspeed = cvar->FindVar("cl_forwardspeed");

                        auto negative_forward_speed = -cl_forwardspeed->GetFloat();
                        auto negative_side_speed = -cl_sidespeed->GetFloat();

                        auto negative_forward_direction = forward * negative_forward_speed;
                        auto negative_side_direction = forward * negative_side_speed;

                        cmd->forwardmove = negative_forward_direction.x;
                        cmd->sidemove = negative_side_direction.y;
                }
}
*/
