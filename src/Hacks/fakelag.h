#pragma once

#include "../SDK/IInputSystem.h"

namespace FakeLag
{
	inline static bool should_recharge;
	//Hooks
	void CreateMove(CUserCmd* cmd);
	inline int ticks = 0;
	inline int shift = 0;
};
