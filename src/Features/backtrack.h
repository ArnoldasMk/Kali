#pragma once

#include <algorithm>
#include <deque>

#include "../SDK/IEngineClient.h"
#include "../interfaces.h"
#include "../Utils/entity.h"

#ifndef CLAMP
#define CLAMP(x, upper, lower) (std::min(upper, std::max(x, lower)))
#endif

struct tickInfo
{
    int TickCount;
    float simulationTime;
    Vector origin;
    matrix3x4_t BoneMatrix[128];
};

class BackTrack
{
private:
    struct cvars
    {
        ConVar *updateRate;
        ConVar *maxUpdateRate;
        ConVar *minUpdateRate;
        ConVar *interp;
        ConVar *interpRatio;
        ConVar *minInterpRatio;
        ConVar *maxInterpRatio;
        ConVar *maxUnlag;
    } cvars;

    float LerpTime;

public:
    inline BackTrack(void);

    inline float SetLerpTime();
    inline bool IsTickValid();
    inline void RemoveInvalidTicks();
    inline void StoreTicks(C_BasePlayer *enemy);
    inline void TimeToTicks(CUserCmd *cmd);

    // void CreatMove(CUserCmd* cmd);
    // void FrameStageNotify(CUserCmd* cmd);

    // std::deque<BackTrack::tickInfo> BackTrackRecords;
    tickInfo record;
};

// constructor
inline BackTrack::BackTrack(void)
{

    this->cvars.updateRate = cvar->FindVar("cl_updaterate");
    this->cvars.maxUpdateRate = cvar->FindVar("sv_maxupdaterate");
    this->cvars.minUpdateRate = cvar->FindVar("sv_maxupdaterate");
    this->cvars.interp = cvar->FindVar("cl_interp");
    this->cvars.interpRatio = cvar->FindVar("cl_interp_ratio");
    this->cvars.minInterpRatio = cvar->FindVar("sv_client_min_interp_ratio");
    this->cvars.maxInterpRatio = cvar->FindVar("sv_client_max_interp_ratio");
    this->cvars.maxUnlag = cvar->FindVar("sv_maxunlag");
}

inline float BackTrack::SetLerpTime()
{
    if (this->cvars.minUpdateRate && this->cvars.maxUpdateRate)
        this->cvars.updateRate->SetValue(this->cvars.maxUpdateRate->GetInt());

    if (!this->cvars.interpRatio)
        this->cvars.interpRatio->SetValue(float(1));

    if (this->cvars.minInterpRatio && this->cvars.maxInterpRatio && this->cvars.minInterpRatio->GetFloat() != 1)
        this->cvars.interpRatio->SetValue(CLAMP(this->cvars.interpRatio->GetFloat(), this->cvars.minInterpRatio->GetFloat(), this->cvars.maxInterpRatio->GetFloat()));

    LerpTime = std::max(this->cvars.interp->GetFloat(), (this->cvars.interpRatio->GetFloat() / this->cvars.updateRate->GetInt()));
}

inline bool BackTrack::IsTickValid()
{
    float &time = this->record.simulationTime;
    float correct = 0.f;
    this->SetLerpTime();
    correct += LerpTime;
    correct = CLAMP(correct, 0.f, this->cvars.maxUnlag->GetFloat());

    float deltaTime = correct - (globalVars->curtime - time);

    if (fabsf(deltaTime) < 0.2f) // fabsf - Computes the absolute value of a floating point value arg more info in -> "https://en.cppreference.com/w/c/numeric/math/fabs"
        return true;

    return false;
}

inline void BackTrack::StoreTicks(C_BasePlayer *enemy)
{

    this->record.origin = enemy->GetAbsOrigin();
    this->record.simulationTime = enemy->GetSimulationTime();
    this->record.TickCount = globalVars->tickcount;
    enemy->SetupBones(this->record.BoneMatrix, 128, 256, globalVars->curtime);
}

inline void BackTrack::TimeToTicks(CUserCmd *cmd)
{
    this->SetLerpTime();
    float time = this->record.simulationTime + this->LerpTime;
    cmd->tick_count = static_cast<int>(0.5f + time / globalVars->interval_per_tick);
}