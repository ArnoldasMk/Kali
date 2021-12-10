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
#define TICK_INTERVAL (globalVars->interval_per_tick)
#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / TICK_INTERVAL))
#define TICKS_TO_TIME(t) (TICK_INTERVAL * (t))
#include "autowall.h"
#include "lagcomp.h"

class RagebotPredictionSystem
{
#define RandomeFloat(x) (static_cast<double>(static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX / x)))
public:
	RagebotPredictionSystem() {}
	float normalize_pitch(float pitch)
	{
		while (pitch > 89.0f)
			pitch -= 180.0f;

		while (pitch < -89.0f)
			pitch += 180.0f;

		return pitch;
	}

	Vector CalculateAngle(Vector src, Vector dst)
	{
		Vector angles;

		Vector delta = src - dst;
		float hyp = delta.Length2D();

		angles.y = std::atanh(delta.y / delta.x) * 57.2957795131f;
		angles.x = std::atanh(-delta.z / hyp) * -57.2957795131f;
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}

	void BestHeadPoint(C_BasePlayer *player, const int &BoneIndex, int &Damage, Vector &Spot)
	{
		matrix3x4_t matrix[128];
		if (!player->SetupBones(matrix, 128, 0x100, 0.f))
			return;
		model_t *pModel = player->GetModel();
		if (!pModel)
			return;

		studiohdr_t *hdr = modelInfo->GetStudioModel(pModel);
		if (!hdr)
			return;
		mstudiobbox_t *bbox = hdr->pHitbox((int)Hitbox::HITBOX_HEAD, 0);
		if (!bbox)
			return;

		Vector mins, maxs;
		Math::VectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
		Math::VectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);
		auto local = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

		Vector center = (mins + maxs) * 0.5f;
		static Vector points[7] = {center, center, center, center, center, center, center};
		// 0 - center, 1 - skullcap, 3 - upperbackofhead
		// 4 - leftear, 5 - rightear, 6 - backofhead
		for (int i = 0; i < 7; i++) // set all points initially to center mass of head.
			points[i] = center;
		float scale = Settings::Ragebot::HeadScale;
		auto final_radius = bbox->radius * scale;

		auto pitch_down = normalize_pitch(player->GetEyeAngles()->x) > 85.0f;
		float stuff = Math::CalcAngle(player->GetEyePosition(), local->GetAbsOrigin().y).y;
		auto backward = fabs(player->GetEyeAngles()->y - stuff) > 120.0f;

		points[1] = Vector(bbox->bbmax.x + 0.70710678f * final_radius, bbox->bbmax.y - 0.70710678f * final_radius, bbox->bbmax.z);
		points[3] = Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + final_radius);
		points[4] = Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - final_radius);
		points[5] = Vector(bbox->bbmax.x, bbox->bbmax.y - final_radius, bbox->bbmax.z);

		if (pitch_down && backward)
			points[6] = Vector(bbox->bbmax.x - final_radius, bbox->bbmax.y, bbox->bbmax.z);

		for (int i = 0; i < 7; i++)
		{
			float bestDamage = AutoWall::GetDamage(points[i], true);
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
	void BestMultiPoint(C_BasePlayer *player, int &BoneIndex, int &Damage, Vector &Spot)
	{
		model_t *pModel = player->GetModel();
		if (!pModel)
			return;
		studiohdr_t *hdr = modelInfo->GetStudioModel(pModel);
		if (!hdr)
			return;
		mstudiobbox_t *bbox = hdr->pHitbox((int)BoneIndex, 0);
		if (!bbox)
			return;

		matrix3x4_t matrix[128];
		Vector mins, maxs;
		Math::VectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
		Math::VectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);
		// 0 - center 1 - left, 2 - right, 3 - back
		Vector center = (mins + maxs) * 0.5f;
		Vector points[4] = {center, center, center, center};
		float scale = Settings::Ragebot::BodyScale;
		auto final_radius = bbox->radius * scale;

		points[1] = Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + final_radius);
		points[2] = Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - final_radius);
		points[3] = Vector(center.x, bbox->bbmax.y - final_radius, center.z);

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

	bool canShoot(CUserCmd *cmd, C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon, Vector &bestSpot, C_BasePlayer *enemy, const RageWeapon_t &currentSettings)
	{
		if (!enemy || !enemy->GetAlive())
			return false;
		if ((*csGameRules)->IsFreezeTime())
			return false;
		auto servertime = TICKS_TO_TIME(localplayer->GetTickBase());
		if (servertime < activeWeapon->GetNextPrimaryAttack())
			return false;
		if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER && activeWeapon->GetPostPoneReadyTime() > servertime)
			return false;
		if (activeWeapon->GetAmmo() == 0)
			return false;
		if (currentSettings.HitChance == 0)
			return true;
		if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 && !(localplayer->GetFlags() & FL_ONGROUND) && fabs(localplayer->GetVelocity().z) < 5.0f && localplayer->GetVelocity().Length2D() < 5.0f)
			return true;
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

		for (int i = 0; i < 255; i++)
		{
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

			if (static_cast<int>((hitCount / 255.f) * 100.f) >= currentSettings.HitChance)
				return true;

			if ((255 - i + hitCount) < NeededHits)
				return false;
		}

		return false;
	}
};
namespace Ragebot
{
	void CheckHit(C_BaseCombatWeapon *);
	inline std::string misstring;
	inline bool miss = false;
	struct enemy
	{
		C_BasePlayer *player = nullptr;
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
	void quickpeek(CUserCmd *);
	void gotoStart(CUserCmd *);
	void CreateMove(CUserCmd *);
	void FireGameEvent(IGameEvent *event);
	void UpdateValues();
	inline RagebotPredictionSystem *ragebotPredictionSystem = new RagebotPredictionSystem();
}
