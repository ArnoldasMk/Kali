#include "animfix.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"
#include "../Utils/draw.h"
#include "../SDK/vector.h"
#define TICK_INTERVAL                   (globalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )             ( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )              ( TICK_INTERVAL *( t ) )

bool DidHit(trace_t trace)
{
        return trace.fraction < 1.0f || trace.allsolid || trace.startsolid;
}

void Extrapolate(C_BasePlayer* player, Vector origin, Vector velocity, int flags, bool on_ground)
{
    static const auto sv_gravity = cvar->FindVar("sv_gravity");
    static const auto sv_jump_impulse = cvar->FindVar("sv_jump_impulse");

    if (!(flags & FL_ONGROUND))
        velocity.z -= TICKS_TO_TIME(sv_gravity->GetFloat());
    else if (player->GetFlags() & FL_ONGROUND && !on_ground)
        velocity.z = sv_jump_impulse->GetFloat();

    const auto src = origin;
    auto end = src + velocity * TICK_INTERVAL;

    Ray_t r;
    r.Init(src, end, player->GetMins(), player->GetMaxs());

    trace_t t;
    CTraceFilter filter;
    filter.pSkip = player;

    trace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

    if (t.fraction != 1.f)
    {
        for (auto i = 0; i < 2; i++)
        {
            velocity -= t.plane.normal * velocity.Dot(t.plane.normal);

            const auto dot = velocity.Dot(t.plane.normal);
            if (dot < 0.f)
                velocity -= Vector(dot * t.plane.normal.x,
                    dot * t.plane.normal.y, dot * t.plane.normal.z);

            end = t.endpos + velocity * TICKS_TO_TIME(1.f - t.fraction);

            r.Init(t.endpos, end, player->GetMins(), player->GetMaxs());
            trace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

            if (t.fraction == 1.f)
                break;
        }
    }

    origin = end = t.endpos;
    end.z -= 2.f;

    r.Init(origin, end, player->GetMins(), player->GetMaxs());
    trace->TraceRay(r, MASK_PLAYERSOLID, &filter, &t);

    flags &= ~FL_ONGROUND;

    if (DidHit(t) && t.plane.normal.z > .7f)
        flags |= FL_ONGROUND;
}

void AnimFix::FrameStageNotify(ClientFrameStage_t stage)
{
    if (!engine->IsInGame())
        return;

        C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

   if (!localplayer || !localplayer->GetAlive())
        return;
/*
static auto backup_abs = localplayer->GetAnimState()->goalFeetYaw;
if (!input->m_fCameraInThirdPerson){
localplayer->ClientAnimations(true);
localplayer->updateClientAnimation();
localplayer->ClientAnimations(false);
	return;
}
    static int old_tick = 0;
    if (old_tick != globalVars->tickcount)
    {
	old_tick = globalVars->tickcount;
	localplayer->ClientAnimations(true);
	localplayer->updateClientAnimation();
	localplayer->ClientAnimations(false);
	if (CreateMove::sendPacket)
	{
	backup_abs = localplayer->GetAnimState()->goalFeetYaw;
	}
   }
  localplayer->GetAnimState()->goalFeetYaw = 0.f;
//  localplayer->SetAbsOrigin(Vector(0, backup_abs,0 ));
*/
               int maxClient = engine->GetMaxClients();
               for (int i = 1; i < maxClient; ++i)
                {
                        //indx = i;
                        C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

                        if (!player 
                        || player == localplayer 
                        || player->GetDormant() 
                        || !player->GetAlive() 
                        || player->GetImmune()
                        || Entity::IsTeamMate(player, localplayer))
                                continue;

			Extrapolate(player, player->GetVecOrigin(), player->GetVelocity(), player->GetFlags(), player->GetFlags() & FL_ONGROUND);
		}
}
