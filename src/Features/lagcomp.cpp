#include "lagcomp.h"

#include "../Utils/math.h"
#include "../interfaces.h"
#include "../settings.h"
#include "../SDK/INetChannel.h"

#ifndef CLAMP
    #define CLAMP(x, upper, lower) (std::min(upper, std::max(x, lower)))
#endif

std::vector<LagComp::LagCompTickInfo> LagComp::lagCompTicks;


float LagComp::GetLerpTime()
{
	int updateRate = cvar->FindVar("cl_updaterate")->GetInt();
	ConVar *minUpdateRate = cvar->FindVar("sv_minupdaterate");
	ConVar *maxUpdateRate = cvar->FindVar("sv_maxupdaterate");

	if (minUpdateRate && maxUpdateRate)
		updateRate = maxUpdateRate->GetInt();

	float ratio = cvar->FindVar("cl_interp_ratio")->GetFloat();
	if (ratio == 0)
		ratio = 1.0f;

	float lerp = cvar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = cvar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = cvar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = CLAMP(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return std::max(lerp, (ratio / updateRate));
}

static bool IsTickValid(float time) // pasted from polak getting some invalid ticks need some fix
{
	float correct = 0;
	float bt_time = Settings::Backtrack::time;

	correct += LagComp::GetLerpTime();
	correct = CLAMP(correct, 0.f, cvar->FindVar("sv_maxunlag")->GetFloat());

	float deltaTime = correct - (globalVars->curtime - time);

	if (Settings::Ragebot::enabled)
		bt_time = Settings::CVarsOverride::fakeLatency ? 0.4f : 0.2f; // use max time

	if (fabsf(deltaTime) < bt_time)
		return true;

	return false;
}

static void RemoveInvalidTicks()
{
	auto &records = LagComp::lagCompTicks;

	for (auto record = records.begin(); record != records.end(); record++)
	{
		if (!IsTickValid(record->simulationTime))
		{
			records.erase(record);

			if (!records.empty())
				record = records.begin();
			else
				break;
		}
	}
}

static void RegisterTicks()
{
	const auto localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	const auto curTick = LagComp::lagCompTicks.insert(LagComp::lagCompTicks.begin(), {globalVars->tickcount, globalVars->curtime});
	
	for (int i = engine->GetMaxClients(); i > 1 ; i--)
	{
		const auto player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player
		|| player == localplayer
		|| player->GetDormant()
		|| !player->GetAlive()
		|| Entity::IsTeamMate(player, localplayer)
		|| player->GetImmune())
			continue;

		LagComp::LagCompRecord record;

		record.entity = player;
		record.origin = player->GetVecOrigin();
		record.head = player->GetBonePosition(BONE_HEAD);

		if (player->SetupBones(record.bone_matrix, 128, BONE_USED_BY_HITBOX, globalVars->curtime))
			curTick->records.push_back(record);
	}
}

void LagComp::CreateMove(CUserCmd *cmd)
{
	if (!Settings::Backtrack::enabled || !Settings::Ragebot::enabled)
		return;

	RemoveInvalidTicks();
	RegisterTicks();

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	C_BaseCombatWeapon *weapon = (C_BaseCombatWeapon *)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!weapon)
		return;

	float serverTime = localplayer->GetTickBase() * globalVars->interval_per_tick;
	float fov = 7.0f;
	static int tickcount = cmd->tick_count;
	bool has_target = false;

	for (auto &&Tick : LagComp::lagCompTicks)
	{
		for (auto &record : Tick.records)
		{
			float tmpFOV = Math::GetFov(cmd->viewangles, Math::CalcAngle(localplayer->GetEyePosition(), record.head));

			if (tmpFOV < fov)
			{
				fov = tmpFOV;
				tickcount = Tick.tickCount;
				has_target = true;
			}
		}
	}

	if (has_target)
		cmd->tick_count = tickcount;
}
