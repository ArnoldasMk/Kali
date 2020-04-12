#pragma once

#include <vector>
#include <cstdint>
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"

namespace Ragebot {

    extern std::vector<int64_t> friends;
    extern int targetAimbot;

    void CreateMove(CUserCmd*);
    void FireGameEvent(IGameEvent* event);
	void UpdateValues();
}

