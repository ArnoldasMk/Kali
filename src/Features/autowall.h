#pragma once

#include "../SDK/vector.h"
#include "../SDK/IEngineTrace.h"
#include "legitbot.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"

namespace AutoWall
{
	struct FireBulletData
	{
		Vector src;
		trace_t enter_trace;
		Vector direction;
		CTraceFilter filter;
		float trace_length;
		float trace_length_remaining;
		float current_damage;
		int penetrate_count;
	};

	int GetDamage(const Vector &vecPoint, bool teamCheck);
	float GetDamage(const Vector &point, bool teamCheck, FireBulletData& fdata);
	int GetDamage(C_BasePlayer *player, const Vector &point, bool teamCheck, FireBulletData &fdata);
}
