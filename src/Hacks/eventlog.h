#pragma once

#include "../SDK/IGameEvent.h"
#include <vector>
#include <string>
namespace Eventlog
{

	//Hooks
	void FireGameEvent(IGameEvent* event);
	void Paint();
}
