#pragma once

#include "../SDK/IGameEvent.h"
#include "../SDK/definitions.h"
#include <cstdint>
#include <vector>

#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"
#include "../interfaces.h"
#include "../settings.h"
#include "antiaim.h"

namespace memeangles
{

    void FrameStageNotify(ClientFrameStage_t stage); // This is where the resolver happens.
    void FireGameEvent(IGameEvent *event);                       // Strangely, I never see this getting called.
    void CreateMove(CUserCmd* cmd);
    void AnimationFix(C_BasePlayer *player);
//      void PostFrameStageNotify(ClientFrameStage_t stage);
} 
