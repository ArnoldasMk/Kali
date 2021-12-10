#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/CViewRender.h"
#include "../SDK/IGameEvent.h"
namespace AntiAim
{
    inline QAngle realAngle;
    inline QAngle fakeAngle;
    extern QAngle LastTickViewAngle;
    inline float realDuck;
    float GetMaxDelta( CCSGOAnimState *animState );
    namespace ManualAntiAim
    {
        inline bool alignBack = false, 
                    alignRight = false, 
                    alignLeft = false;
    }
    
    inline bool bSend = true;
    inline bool LbyUpdate();
    inline bool casualcheck();
    //Hooks
    void CreateMove(CUserCmd* cmd);
    void FrameStageNotify(ClientFrameStage_t stage);
    void OverrideView(CViewSetup *pSetup);
    void FireGameEvent(IGameEvent* event);
}
