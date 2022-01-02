#pragma once

#include <vector>
#include <cstdint>
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../SDK/definitions.h"
#include "../Utils/xorstring.h"
#include "../SDK/IClientEntity.h"

namespace Legitbot
{
	extern bool aimStepInProgress;
	extern std::vector<int64_t> friends;
	extern int targetAimbot;
	extern C_BasePlayer *curtarget;

	// Hooks
	void CreateMove(CUserCmd *cmd);
	void FireGameEvent(IGameEvent *event);
	void UpdateValues();
	bool HitChance(Vector bestSpot, C_BasePlayer *player, C_BaseCombatWeapon *activeWeapon, float hitChance);
}
