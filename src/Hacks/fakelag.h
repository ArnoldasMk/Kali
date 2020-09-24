#pragma once

#include "../SDK/IInputSystem.h"

namespace FakeLag
{
	//Hooks
	void CreateMove(CUserCmd* cmd);
	inline int ticks = 0;
};
