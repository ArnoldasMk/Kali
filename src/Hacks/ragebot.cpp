#pragma GCC diagnostic ignored "-Wcomment"
#pragma GCC diagnostic ignored "-Warray-bounds"

#include "ragebot.h"
#include "autowall.h"

#include "../Utils/bonemaps.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"
#include "backtrack.h"

#include <thread>
#include <future>
#include <iostream>
#include <stdlib.h>

#define PI_F (3.14)
#define absolute(x) ( x = x < 0 ? x * -1 : x)
#define TICK_INTERVAL			(globalVars->interval_per_tick)


#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#ifndef NormalizeNo
	#define NormalizeNo(x) (x = (x < 0) ? ( x * -1) : x )
#endif

#ifndef GetPercentVal
    #define GetPercentVal(val, percent) ( val * (percent/100) )
#endif

#ifndef GetPercent
	#define GetPercent(total, val) ( (val/total) * 100)
#endif

std::vector<int64_t> Ragebot::friends = {};
std::vector<long> RagebotkillTimes = { 0 }; // the Epoch time from when we kill someone

inline bool	doubleFire = false,
			DieBitch = false;

const int MultiVectors = 7;

static void VelocityExtrapolate(C_BasePlayer* player, Vector& aimPos){	
	if( player->GetVelocity().Length() < 135.f )
		return;
	aimPos += (player->GetVelocity() * globalVars->interval_per_tick);	
}

/* Fills points Vector. True if successful. False if not.  Credits for Original method - ReactiioN */
static void BestMultiPointDamage(C_BasePlayer* player, int BoneIndex, matrix3x4_t boneMatrix[], int& Damage, Vector& Spot)
{

	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
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

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

	float bestDamage = 0.f;
	// 0 - center, 1 - forehead, 2 - skullcap,
	// 3 - leftear, 4 - rightear, 5 - nose, 6 - backofhead
	Vector center = player->GetBonePosition(BoneIndex);
	Vector points[MultiVectors] = { center,center,center,center,center,center,center };

    points[1].z += bbox->radius * 0.60f; // morph each point.
	points[2].z += bbox->radius * 1.35f; // ...
	points[3].y -= bbox->radius * 0.65f;
	points[3].z += bbox->radius * 0.90f;
	points[4].x += bbox->radius * 0.80f;
	points[5].x -= bbox->radius * 0.80f;
	points[6].y += bbox->radius * 0.80f;

	for (int i = 0; i < MultiVectors; i++)
	{
		AutoWall::FireBulletData data;
		bestDamage = AutoWall::GetDamage(points[i], true, data);
		if (bestDamage >= localplayer->GetHealth())
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

/*
 * Method for safety damage prediction where 
 * It will just look for required Damage Not for the best damage
 */
static void GetDamageAndSpots(C_BasePlayer* player, Vector &Spot, int& Damage, const int& playerHelth, int& i)
{
	using namespace Settings;
	Damage = 0.f;
	Spot = Vector(0);
	
	if (!Settings::Ragebot::AutoAim::desireBones[i])	return;

	matrix3x4_t boneMatrix[128];

	if ( !player->SetupBones(boneMatrix, 128, 0x100, 0) )
		return;

	int boneID = -1;

	switch ((DesireBones)i)
	{
		case DesireBones::BONE_HEAD:
			boneID = CONST_BONE_HEAD;
			break;
		case DesireBones::UPPER_CHEST:
			boneID = CONST_BONE_UPPER_SPINE;
			break;
		case DesireBones::MIDDLE_CHEST:
			boneID = CONST_BONE_MIDDLE_SPINE;
			break;
		case DesireBones::LOWER_CHEST:
			boneID = CONST_BONE_LOWER_SPINE;
			break;
		case DesireBones::BONE_HIP:
			boneID = CONST_BONE_HIP;
			break;
		case DesireBones::LOWER_BODY:
			boneID = BONE_PELVIS;
			break;
		default:
			return;
	}

	if (boneID == BONE_INVALID) // bone not available on this modeltype.
	   	return;
	else if (boneID == CONST_BONE_HEAD)
		BestMultiPointDamage(player, boneID, boneMatrix, Damage, Spot);
	else 
	{
		const Vector &bone3D = player->GetBonePosition(boneID);
		static AutoWall::FireBulletData data;
		Damage = AutoWall::GetDamage(bone3D, true, data);
		Spot = bone3D;
    }
	
}

/*
 * Get best Damage from the enemy and the spot
 */
static void GetBestSpotAndDamage(C_BasePlayer* player, Vector& Spot, int& Damage)
{
	const int &len = 5; // Atleast Now Total Bones we are caring of
	Vector spot1 = Vector(0), spot2 = Vector(0);
	int damage1 = 0.f, damage2 = 0.f;

	const int &playerHelth = player->GetHealth();

	if (Settings::Ragebot::damagePrediction == DamagePrediction::justDamage)
	{	
		int start = 0, 
			end = len/2;
		do
		{
			// Using Thread and don't ask me why but it cause a huge performance drop anyway for multiple enemies
			std::thread([&](){ GetDamageAndSpots(player, spot1, damage1, playerHelth, start); }).join();
			
			std::thread([&](){ GetDamageAndSpots(player, spot2, damage2, playerHelth, end); }).join();

			start++;

			if ( damage1 >= Settings::Ragebot::MinDamage || damage2 >= Settings::Ragebot::MinDamage )
			{
				if (damage1 >= playerHelth)
				{
					Damage = damage1;
					Spot = spot1;
					return;
				}
				else if (damage2 >= playerHelth )
				{
					Damage = damage1;
					Spot = spot1;
					return;
				}
				else if (damage1 >= damage2 && damage1 > Damage)
				{
					Damage = damage1;
					Spot = spot1;
					return;
				}
				else if ( damage2 > Damage)
				{
					Damage = damage2;
					Spot = spot2;
					return;
				}
			}
		}while (start <= end);	
	}
	
	else if (Settings::Ragebot::damagePrediction == DamagePrediction::damage)
	{
		int start = 0, end = len/2;
		do
		{
			std::thread([&](){ GetDamageAndSpots(player, spot1, damage1, playerHelth, start); }).join();
			std::thread([&](){ GetDamageAndSpots(player, spot2, damage2, playerHelth, end); }).join();
			start++;
			if ( damage1 >= Settings::Ragebot::MinDamage && damage1 >= damage2 )
			{
				if (damage1 >= playerHelth)
				{
					Damage = damage1;
					Spot = spot1;
					return;
				}
				else if (damage1 > Damage)
				{
					Spot = spot1;
					Damage = damage1;
					continue;
				}
			}
			else if ( damage2 >= Settings::Ragebot::MinDamage && damage2 >= damage1)
			{
				if (damage1 >= playerHelth)
				{
					Damage = damage2;
					Spot = spot2;
					return;
				}
				else if (damage2 > Damage)
				{
					Damage = damage2;
					Spot = spot2;
				}
			}
		}while (start <= end);

		cvar->ConsoleDPrintf(XORSTR("Damage : %d\n"), Damage);
		// cheaking for the middle number		
	}

}

/*
 * To find the closesnt enemy to reduce the calculation time and increase performace
 * Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) 
 */
static C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer, CUserCmd* cmd)
{
	C_BasePlayer* closestPlayer = nullptr;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);
	float prevFOV = 0.f;

	for (int i = engine->GetMaxClients(); i > 1; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*)entityList->GetClientEntity(i);

		if (!player
	    	|| player == localplayer
	    	|| player->GetDormant()
	    	|| !player->GetAlive()
	    	|| player->GetImmune())
	    	continue;

		if (!Settings::Ragebot::friendly && Entity::IsTeamMate(player, localplayer))
	   	 	continue;

		Vector cbVecTarget = player->GetEyePosition();
		if (Entity::IsSpotVisible(player, cbVecTarget));
		
		float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
	
		if ( cbFov < prevFOV || prevFOV == 0.f)
		{
			prevFOV = cbFov;
			closestPlayer = player;
		}
	}
	return closestPlayer;
}

// get the clossest player from crosshair
static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd,C_BasePlayer* localplayer, Vector& bestSpot, int& bestDamage)
{
	if (!localplayer->GetAlive() || !localplayer)
		return nullptr;

	C_BasePlayer* player = GetClosestEnemy(localplayer, cmd);
	
	if ( !player )
		return nullptr;
			
	GetBestSpotAndDamage(player, bestSpot, bestDamage);
	
	if ( bestDamage >= Settings::Ragebot::MinDamage)
		return player;

	return nullptr;
}

// Get the best damage and the player 
static C_BasePlayer* GetBestEnemyAndSpot(CUserCmd* cmd,C_BasePlayer* localplayer, Vector& BestSpot, int& BestDamage)
{
	if (!localplayer)
		return nullptr;
	if (!localplayer->GetAlive())
		return nullptr;

	Vector bestSpot = Vector(0);
	int bestDamage = 0;
	C_BasePlayer* clossestEnemy = nullptr;
	for (int i = engine->GetMaxClients(); i  > 1; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			player == localplayer || 
			player->GetDormant() || 
			!player->GetAlive() || 
			player->GetImmune())
			continue;

		if (Entity::IsTeamMate(player, localplayer)) // Checking for Friend If any it will continue to next player
			continue;
		GetBestSpotAndDamage(player, bestSpot, bestDamage);
		if (bestDamage >= localplayer->GetHealth() )
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return player;
		}	
		else if (bestDamage > BestDamage){
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			clossestEnemy = player;
		}
	}	

	if (BestSpot.IsZero() || BestDamage < 0)
		return nullptr;

	return clossestEnemy;
}

//Hitchance source from nanoscence
static float Ragebothitchance(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, QAngle& angle, C_BasePlayer* enemy)
{
	float hitchance = 10;
	
	if (!activeWeapon)	return hitchance;

	activeWeapon->UpdateAccuracyPenalty();
	hitchance = activeWeapon->GetInaccuracy();
	if (hitchance == 0) hitchance = 0.0000001;
	hitchance = 1 / hitchance + activeWeapon->GetSpread();
	
	return hitchance;
}

static void RagebotNoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
    if (!(cmd->buttons & IN_ATTACK) || 
		!localplayer || 
		!localplayer->GetAlive())
		return;

	const float& aimpunch = cvar->FindVar("weapon_recoil_scale")->GetFloat();
	
	if (!aimpunch || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
		return;
	
	const QAngle& CurrentPunch = *localplayer->GetAimPunchAngle();
	angle.x -= CurrentPunch.x * 2.f;
	angle.y -= CurrentPunch.y * 2.f;
}

// AutoCroutch is a bad idea in hvh instant death if you miss
static void RagebotAutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon)
{
    if (!localplayer || !localplayer->GetAlive() || !Settings::Ragebot::AutoCrouch::enable)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK | IN_BULLRUSH;
}

static void RagebotAutoSlow(C_BasePlayer* localplayer, C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, QAngle& angle)
{
	if (!Settings::Ragebot::AutoSlow::enabled)
		return;
	
	if (!localplayer)
		return;
	if (!localplayer->GetAlive())
		return;
	
	if (!activeWeapon)
		return;
	if (activeWeapon->GetInReload())
		return;

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;
	
	const float &hc = Ragebothitchance(localplayer,activeWeapon, angle, player);

	if (hc <= (Settings::Ragebot::HitChance::value * 1.5))
	{
		float curTime = globalVars->curtime;
		QAngle ViewAngle = cmd->viewangles;
		Math::ClampAngles(ViewAngle);
		static Vector oldOrigin = localplayer->GetAbsOrigin();
		Vector velocity = (localplayer->GetVecOrigin()-oldOrigin) * (1.f/globalVars->interval_per_tick);
		oldOrigin = localplayer->GetAbsOrigin();
		const float &speed  = velocity.Length();
		
		if(speed > 15.f)
		{
			QAngle dir;
			Math::VectorAngles(velocity, dir);
			dir.y = ViewAngle.y - dir.x;

			Vector NewMove = Vector(0);
			Math::AngleVectors(dir, NewMove);
			const auto &max = std::max(forrwordMove, sideMove);
			const auto &mult = 450.f/max;
			NewMove *= -mult;
			
			forrwordMove = NewMove.x;
			sideMove = NewMove.y;
		}
		else
		{
			forrwordMove = 0.f;
			sideMove = 0.f;
		}
	}
}

static void RagebotAutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
    if (!Settings::Ragebot::AutoPistol::enabled)
    	return;

    if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;

    if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
		return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
		cmd->buttons &= ~IN_ATTACK;
}

static void RagebotAutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,const Vector& bestspot, QAngle& angle)
{
    if (!Settings::Ragebot::AutoShoot::enabled)
		return;
	if (!activeWeapon)
		return;
	if (activeWeapon->GetInReload())
		return;
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
	{		
		cmd->buttons |= IN_ATTACK;

    	float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
		if (postponeFireReadyTime > 0 && postponeFireReadyTime < globalVars->curtime && !player)
			cmd->buttons &= ~IN_ATTACK;

		return;
	}

	float hc = Ragebothitchance(localplayer, activeWeapon, angle, player);
	
	if (Settings::Ragebot::HitChanceOverwrride::enable)
		hc -= GetPercentVal(hc, Settings::Ragebot::HitChanceOverwrride::value);
	
	if ( hc <= Settings::Ragebot::HitChance::value * 1.5)
		return;	

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
	    cmd->buttons &= ~IN_ATTACK;
	else
		cmd->buttons |= IN_ATTACK;
}

static void FixMouseDeltas(CUserCmd* cmd, C_BasePlayer* player, const QAngle& angle, const QAngle& oldAngle)
{
    if (!player)
		return;
	if (!player->GetAlive())
		return;

    QAngle delta = angle - oldAngle;
    float sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
    float m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
    float m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
    float zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

    Math::NormalizeAngles(delta);

    cmd->mousedx = -delta.y / (m_yaw * sens * zoomMultiplier);
    cmd->mousedy = delta.x / (m_pitch * sens * zoomMultiplier);
}


void Ragebot::CreateMove(CUserCmd* cmd)
{
	if (!Settings::Ragebot::enabled)
		return;

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer || !localplayer->GetAlive())
		return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon || activeWeapon->GetInReload())
		return;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;
	
	if (Ragebot::prevWeapon != (ItemDefinitionIndex)*activeWeapon->GetItemDefinitionIndex())
	{
		Ragebot::prevWeapon = (ItemDefinitionIndex)*activeWeapon->GetItemDefinitionIndex();
		Ragebot::UpdateValues();
	}

    QAngle oldAngle;
    engine->GetViewAngles(oldAngle);
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;

	Vector localEye = localplayer->GetEyePosition();
    QAngle angle = cmd->viewangles;

    Vector bestSpot = Vector(0);
	int bestDamage = 0;

	C_BasePlayer* player = nullptr;
	switch (Settings::Ragebot::enemySelectionType)
	{
		case EnemySelectionType::BestDamage :
			player = GetBestEnemyAndSpot(cmd, localplayer, bestSpot, bestDamage);
			break;
		case EnemySelectionType::CLosestToCrosshair :
			player = GetClosestPlayerAndSpot(cmd, localplayer, bestSpot, bestDamage);
			break;
		default:
			break;
	}

    if (player && bestDamage > 0)
    {	
		if (Settings::Ragebot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) )
	    {
			cmd->buttons |= IN_ATTACK2;
			return;
	    }
		Settings::Debug::AutoAim::target = bestSpot;
		VelocityExtrapolate(player, localEye);
		VelocityExtrapolate(player, bestSpot);
		angle = Math::CalcAngle(localEye, bestSpot);

		RagebotAutoSlow(localplayer, player, activeWeapon, cmd, oldForward, oldSideMove, angle);
    	RagebotAutoCrouch(player, cmd, activeWeapon);
    	RagebotAutoPistol(activeWeapon, cmd);
    	RagebotAutoShoot(player,localplayer, activeWeapon, cmd, bestSpot, angle);
    	RagebotNoRecoil(angle, cmd, localplayer, activeWeapon);
    }

    Math::NormalizeAngles(angle);
    // Math::ClampAngles(angle);

    FixMouseDeltas(cmd, player, angle, oldAngle);
    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
	
    // if (!Settings::Ragebot::silent)
	// 	engine->SetViewAngles(cmd->viewangles);
}

void Ragebot::FireGameEvent(IGameEvent* event)
{
	if(!Settings::Ragebot::enabled || !event)
		return;
		
    if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0)
    {
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
	    	return;
		Ragebot::friends.clear();
    }
    if (strcmp(event->GetName(), XORSTR("player_death")) == 0)
    {
		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) // suicide
	    	return;

		if (attacker_id != engine->GetLocalPlayer())
	    	return;

		RagebotkillTimes.push_back(Util::GetEpochTime());
    }
}

