#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/CViewRender.h"
#include "../SDK/IGameEvent.h"

namespace AntiAim
{
    extern QAngle realAngle;
    extern QAngle fakeAngle;

    float GetMaxDelta(CCSGOAnimState *animState);

    //Hooks
    void CreateMove(CUserCmd* cmd);
    void FrameStageNotify(ClientFrameStage_t stage);
    void OverrideView(CViewSetup *pSetup);
    void FireGameEvent(IGameEvent* event);
}
