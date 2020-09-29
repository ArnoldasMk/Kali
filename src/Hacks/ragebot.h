#pragma once

#include <vector>
#include <cstdint>
#include <thread>
#include "../settings.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../interfaces.h"
#include "../Utils/xorstring.h"
#include "../hooker.h"
#include "../Hooks/hooks.h"
#include "../Utils/bonemaps.h"
#include "../Utils/math.h"
#include "autowall.h"
#include "lagcomp.h"
#pragma once

#include "autowall.h"
#include "lagcomp.h"

class RagebotPredictionSystem{
    #define RandomeFloat(x) (static_cast<double>( static_cast<double>(std::rand())/ static_cast<double>(RAND_MAX/x)))
public :

    RagebotPredictionSystem(){}

    void BestHeadPoint(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot)
    {
	    if (!player || !player->GetAlive())
		    return;

	    model_t* pModel = player->GetModel();
        if (!pModel)
		    return;
        studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
        if (!hdr)
		    return;
	    mstudiobbox_t* bbox = hdr->pHitbox((int)BoneIndex, 0);
	    if (!bbox)
		    return;

	    // 0 - center, 1 - skullcap,
	    // 2 - leftear, 3 - rightear, 4 - backofhead
	    Vector center = Spot;
	    Vector points[5] = {center,center,center,center,center};

	    points[1].y -= bbox->radius * 0.65f;
	    points[1].z += bbox->radius * 0.90f;
	    points[2].x += bbox->radius * 0.80f;
	    points[3].x -= bbox->radius * 0.80f;
	    points[4].y += bbox->radius * 0.80f;

	    for (int i = 0; i < 5; i++)
	    {
		    float bestDamage = AutoWall::GetDamage(points[i], true);
		    if (bestDamage >= player->GetHealth())
		    {
		    	Damage = bestDamage;
		    	Spot = points[i];
		    	return;
		    }
		    if (bestDamage > Damage)
		    {
		    	Damage = bestDamage;
		    	Spot = points[i];
		    }
	    }
    }

    void BestMultiPoint(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot)
    {
	    if (!player || !player->GetAlive())
		    return;
	    model_t* pModel = player->GetModel();
        if (!pModel)
		    return;
        studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
        if (!hdr)
		    return;
	    mstudiobbox_t* bbox = hdr->pHitbox((int)BoneIndex, 0);
	    if (!bbox)
		    return;
	    // 0 - center 1 - left, 2 - right, 3 - back
	    Vector center = Spot;
	    Vector points[4] = { center,center,center,center };

        points[1].x += bbox->radius * 0.95f; // morph each point.
	    points[2].x -= bbox->radius * 0.95f;
	    points[3].y -= bbox->radius * 0.95f;

	    for (int i = 0; i < 4; i++)
	    {
		    int bestDamage = AutoWall::GetDamage(points[i], true);
		    if (bestDamage >= player->GetHealth())
		    {
			    Damage = bestDamage;
			    Spot = points[i];
			    return;
		    }
		    else if (bestDamage > Damage)
		    {   
			    Damage = bestDamage;
			    Spot = points[i];
		    }
	    }
    }

    bool canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon,Vector &bestSpot, C_BasePlayer* enemy,const RageWeapon_t& currentSettings)
	{
		if (currentSettings.HitChance == 0)
			return true;
		if (!enemy || !enemy->GetAlive())
			return false;

    	Vector src = localplayer->GetEyePosition();
    	QAngle angle = Math::CalcAngle(src, bestSpot);
    	Math::NormalizeAngles(angle);

		Vector forward, right, up;
    	Math::AngleVectors(angle, &forward, &right, &up);

    	int hitCount = 0;
    	int NeededHits = static_cast<int>(255.f * (currentSettings.HitChance / 100.f));

    	activeWeapon->UpdateAccuracyPenalty();
    	float weap_spread = activeWeapon->GetSpread();
    	float weap_inaccuracy = activeWeapon->GetInaccuracy();

    	for (int i = 0; i < 255; i++) {
    	// RandomSeed(i + 1); // if we can't calculate spread like game does, then at least use same functions XD
        	static float val1 = (2.0 * M_PI);
		// float b = RandomFloat(0.f, 2.f * M_PI);
        // float spread = weap_spread * RandomFloat(0.f, 1.0f);
        // float d = RandomFloat(0.f, 2.f * M_PI);
        // float inaccuracy = weap_inaccuracy * RandomFloat(0.f, 1.0f);

			double b = RandomeFloat(val1);
        	double spread = weap_spread * RandomeFloat(1.0f);
        	double d = RandomeFloat(1.0f);
        	double inaccuracy = weap_inaccuracy * RandomeFloat(1.0f);

        	Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

       	 	direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
			direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
			direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
			direction.Normalize();

        	QAngle viewAnglesSpread;
        	Math::VectorAngles(direction, up, viewAnglesSpread);
			Math::NormalizeAngles(viewAnglesSpread);

			Vector viewForward;
			Math::AngleVectors(viewAnglesSpread, viewForward);
			viewForward.NormalizeInPlace();

			viewForward = src + (viewForward * activeWeapon->GetCSWpnData()->GetRange());

        	trace_t tr;
        	Ray_t ray;

       	 	ray.Init(src, viewForward);
        	trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, enemy, &tr);

        	if (tr.m_pEntityHit == enemy)
           	 	hitCount++;

        	if (static_cast<int>((hitCount/255.f) * 100.f) >= currentSettings.HitChance)
				return true;

			if ((255 - i + hitCount) < NeededHits)
				return false;
    	}

    	return false;
	}

};
namespace Ragebot {
void CheckHit(C_BaseCombatWeapon*);

    struct enemy 
    {
        C_BasePlayer* player = nullptr;
        int LockedBone = -1;
        int bestDamage = 0;
        Vector lockedSpot = Vector(0);
        bool shooted = false;
        int playerhelth = 0;
    };

    inline enemy lockedEnemy;
    inline Vector localEye = Vector(0),
    	 BestSpot = Vector(0);
    inline int BestDamage = 0;
    inline bool r8p;
    inline bool shouldAim = false;
    inline bool shouldSlow = false;
    extern std::vector<int64_t> friends;

    inline int prevDamage = 0, dtTick_Count = 0, dtTick_Need = 0;
    inline ItemDefinitionIndex prevWeapon = ItemDefinitionIndex::INVALID;
    void drawStartPos();
    void quickpeek(CUserCmd*);
    void gotoStart(CUserCmd*);
    void CreateMove(CUserCmd*);
    void FireGameEvent(IGameEvent* event);
    inline RagebotPredictionSystem* ragebotPredictionSystem = new RagebotPredictionSystem();
}


