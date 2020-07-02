#pragma once

#include "../SDK/IGameEvent.h"
#include "../SDK/definitions.h"
#include <cstdint>
#include <vector>

namespace Resolver
{

extern std::vector<int64_t> Players;

//Hooks
void FrameStageNotify(ClientFrameStage_t stage); // This is where the resolver happens.
void FireGameEvent(IGameEvent *event);			 // Strangely, I never see this getting called.

} // namespace Resolver
