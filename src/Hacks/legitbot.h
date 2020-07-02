#pragma once

#include <vector>
#include <cstdint>
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../SDK/definitions.h"

namespace Legitbot
{
	extern bool aimStepInProgress;
	extern std::vector<int64_t> friends;
    extern int targetAimbot;
	inline ItemDefinitionIndex prevWeapon = ItemDefinitionIndex::INVALID;

	//Hooks
	void CreateMove(CUserCmd* cmd);
	void FireGameEvent(IGameEvent* event);
	void UpdateValues();
}
