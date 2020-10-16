#pragma once
#include <vector>
#include "../SDK/IGameEvent.h"

namespace Hitmarkers
{
struct impact_info
{
        float x, y, z;
        float time;
};
//        std::vector<impact_info> hitpact;

	//Hooks
	void FireGameEvent(IGameEvent* event);
	void Paint();
}
