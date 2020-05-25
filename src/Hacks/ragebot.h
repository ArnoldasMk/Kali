#pragma once

#include <vector>
#include <cstdint>
#include <thread>
#include "../SDK/IClientEntity.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"

namespace Ragebot {

    inline bool coacking = false;
    extern std::vector<int64_t> friends;
    extern int targetAimbot;
    inline int TotalShoots, ShotHitted;
    inline C_BasePlayer *LockedEnemy;

    void CreateMove(CUserCmd*);
    void FireGameEvent(IGameEvent* event);
	void UpdateValues();
}


