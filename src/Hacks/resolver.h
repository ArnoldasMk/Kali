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
#include "ragebot.h"

namespace Resolver
{
enum class rflag : int
{
	NONE,
        LBY,
        LOW,
	NOTIME,
	AP,
	BRUTE,
	LAA,
};

    struct resolvedPlayers
    {
        C_BasePlayer* enemy;
        int PrevTrueDelta = 0;
        int MissedCount = 0;
	float delta = 0;
	rflag flags = rflag::NONE;
	bool choke = false;
	float oldpitch;
    };

    inline resolvedPlayers players[1000];
    inline std::pair<C_BasePlayer*, int> player;
    inline int TargetID = 0;
    extern std::vector<int64_t> Players;
    //extern int indx;
    //Hooks
    void FrameStageNotify(ClientFrameStage_t stage); // This is where the resolver happens.
    void FireGameEvent(IGameEvent *event);			 // Strangely, I never see this getting called.
    void CreateMove(CUserCmd* cmd);
    void AnimationFix(C_BasePlayer *player);
//	void PostFrameStageNotify(ClientFrameStage_t stage);
} 
