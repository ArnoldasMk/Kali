#pragma once

#include "../SDK/IGameEvent.h"

namespace VoteRevealer
{
    //Hooks
    void BeginFrame(float frameTime);
    void FireGameEvent(IGameEvent* event);
} // namespace VoteCast
