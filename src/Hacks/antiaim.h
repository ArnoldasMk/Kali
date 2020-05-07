#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IClientEntity.h"

namespace AntiAim
{
    inline QAngle realAngle;
    inline QAngle fakeAngle;
    extern QAngle LastTickViewAngle;

    float GetMaxDelta( CCSGOAnimState *animState );

    //Hooks
    void CreateMove(CUserCmd* cmd);
}