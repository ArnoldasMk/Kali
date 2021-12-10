#include <cstddef>
#include "../SDK/INetChannel.h"
#include "esp.h"
#include "autowall.h"
#include "../fonts.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../Utils/bonemaps.h"
#include "../Utils/xorstring.h"
#include "../Hooks/hooks.h"
#include "../SDK/CPlayerResource.h"
#include "bullettracers.h"

//Fuck it, include everything
void cbullet_tracer::log(IGameEvent* event)
{
	//if we receive bullet_impact event
	if (strstr(event->GetName(), "bullet_impact"))
	{
		//get the user who fired the bullet
		auto index = engine->GetPlayerForUserID(event->GetInt("userid"));

		//get local player
 		C_BasePlayer* local = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
		if (!local)
			return;

		//get the bullet impact's position
		Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

		Ray_t ray;
		ray.Init(local->GetEyePosition(), position);

		//skip local player
		CTraceFilter filter;
		filter.pSkip = local;

		//trace a ray
		trace_t tr;
		trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		//use different color when we hit a player
		auto color = Color(255, 255, 0, 220);

		//push info to our vector
		logs.push_back(cbullet_tracer_info(local->GetEyePosition(), position,  globalVars->curtime, color));
	}
}

void cbullet_tracer::render()
{
	if (!Settings::bullettracers::enabled)
		return;

	//get local player
    C_BasePlayer* local = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!local)
		return;

	//loop through our vector
	for (size_t i = 0; i < logs.size(); i++)
	{
		//get the current item
		auto current = logs.at(i);

		//draw a line from local player's head position to the hit point
	        Draw::AddLine(current.src.x , current.src.y, current.dst.x,  current.dst.y, Settings::ESP::manualAAColor.Color() ); 

		//if the item is older than 5 seconds, delete it
		if (fabs(globalVars->curtime - current.time) > 5.f)
			logs.erase(logs.begin() + i);
	}
}
