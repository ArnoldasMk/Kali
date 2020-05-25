#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IClientEntity.h"

namespace AntiAim
{
    inline QAngle realAngle;
    inline QAngle fakeAngle;
    extern QAngle LastTickViewAngle;

    float GetMaxDelta( CCSGOAnimState *animState );
    
    namespace ManualAntiAim
    {
        inline bool alignBack = false, 
                    alignRight = false, 
                    alignLeft = false;
    }
    

    //Hooks
    void CreateMove(CUserCmd* cmd);
}