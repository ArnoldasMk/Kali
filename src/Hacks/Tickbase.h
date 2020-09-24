#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <thread>
#include "../settings.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../interfaces.h"
#include "../Utils/xorstring.h"
#include "../hooker.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"

struct UserCmd;

namespace Tickbase
{
	void shiftTicks(int, CUserCmd*, bool = false) noexcept;
	bool shouldRecharge(CUserCmd* cmd) noexcept;
	void run(CUserCmd*, bool& sendPacket) noexcept;
	static int lastShift{ 0 };
	struct Tick
	{
		int	maxUsercmdProcessticks{ 17 }; //on valve servers this is 8 ticks, always do +1 command
		int ticksAllowedForProcessing{ maxUsercmdProcessticks };
		int chokedPackets{ 0 };
		int fakeLag{ 0 };
		int tickshift{ 0 };
		int tickbase{ 0 };
		int commandNumber{ 0 };
		int ticks{ 0 };
	};
	inline std::unique_ptr<Tick> tick;
}
