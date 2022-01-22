#include "cvars.h"

#include "../interfaces.h"
#include "../settings.h"
#include "../Utils/xorstring.h"


void CVars::FrameStageNotify(ClientFrameStage_t stage)
{
    if (!engine->IsInGame())
        return;

    cvar->FindVar("cl_ragdoll_gravity")->SetValue(Settings::CVarsOverride::ragdoll ? -400 : 400);
    cvar->FindVar("net_fakelag")->SetValue(Settings::CVarsOverride::fakeLatency ? 100 : 0);
    cvar->FindVar("mat_fullbright")->SetValue(Settings::CVarsOverride::fullbright);
    cvar->FindVar("sv_showimpacts")->SetValue(Settings::CVarsOverride::showImpacts);
}
