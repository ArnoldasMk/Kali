#include "legitbot.h"
#include "autowall.h"

#include "../Utils/xorstring.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../Utils/bonemaps.h"
#include "../settings.h"
#include "../interfaces.h"

#include <stdio.h>
#include <string>
#include <iostream>

bool Legitbot::aimStepInProgress = false;
std::vector<int64_t> Legitbot::friends = { };
std::vector<long> killTimes = { 0 }; // the Epoch time from when we kill someone

QAngle AimStepLastAngle;
QAngle RCSLastPunch;

int Legitbot::targetAimbot = -1;
const int headVectors = 11;

static QAngle ApplyErrorToAngle(QAngle* angles, float margin)
{
	QAngle error;
	error.Random(-1.0f, 1.0f);
	error *= margin;
	angles->operator+=(error);
	return error;
}

/*
* get the Distance from your View Point to the enemy
*/
static float GetRealDistanceFOV(float distance, QAngle angle, CUserCmd* cmd)
{
	/*    n
	    w + e
	      s        'real distance'
	                      |
	   a point -> x --..  v
	              |     ''-- x <- a guy
	              |          /
	             |         /
	             |       /
	            | <------------ both of these lines are the same length
	            |    /      /
	           |   / <-----'
	           | /
	          o
	     localplayer
	*/

	Vector aimingAt;
	Math::AngleVectors(cmd->viewangles, aimingAt);
	aimingAt *= distance;

	Vector aimAt;
	Math::AngleVectors(angle, aimAt);
	aimAt *= distance;

	return aimingAt.DistTo(aimAt);
}

static void VelocityExtrapolate(C_BasePlayer* player, Vector& aimPos, Vector& localeye)
{
	aimPos += (player->GetVelocity() * globalVars->interval_per_tick);
	localeye += (player->GetVelocity() * globalVars->interval_per_tick);
}

/*
* Check Certein Position Is in the Fov Or not
*/
static bool IsIzFov(C_BasePlayer* player, CUserCmd *cmd, const Vector& pVecTarget, const Vector& cbVecTarget, AimTargetType aimTargetType = AimTargetType::FOV)
{
	if (!player)
		return false;

	using namespace Settings::Legitbot::AutoAim;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);
	float realDistance = 0.f;
	if (Settings::Legitbot::AutoAim::realDistance)
	{
		aimTargetType = AimTargetType::REAL_DISTANCE;
		float distance = pVecTarget.DistTo(cbVecTarget);
		realDistance = GetRealDistanceFOV(distance, Math::CalcAngle(pVecTarget, cbVecTarget), cmd);	
	}
		
	float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );

	if (cbFov > 0 && cbFov <= fov && aimTargetType == AimTargetType::FOV)
	{
		if ( !Entity::IsVisible(player, (int) Settings::Legitbot::bone, cbFov, true) )
			return false;
		return true;
	}
	else if (cbFov > 0 && cbFov <= realDistance && aimTargetType == AimTargetType::REAL_DISTANCE)
	{
		if ( !Entity::IsVisible(player, (int) Settings::Legitbot::bone, cbFov, true) )
			return false;
		return true;
	}

	return false;
	
}

/* 
* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) 
* It find out he clossest enemy from your View Angle and return the enemy
*/
static C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer, CUserCmd* cmd, AimTargetType aimTargetType = AimTargetType::FOV)
{
	if (!localplayer)
		return nullptr;

	C_BasePlayer* closenstEntity = nullptr;
	float bestFov = Settings::Legitbot::AutoAim::fov;
	float bestRealDistance = Settings::Legitbot::AutoAim::fov * 5.0f;

	float prevDistance = (float)0;

	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	if (Settings::Legitbot::AutoAim::realDistance)
		aimTargetType = AimTargetType::REAL_DISTANCE;

	
	for (int i = engine->GetMaxClients(); i > 1 ; i--)
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

		if (Entity::IsSpotVisible(player, cbVecTarget, bestFov, true))
			return player;

		float cbFov = 0.f;
		float distance = 0.f;
		float realDistance = 0.f;
		
		switch (aimTargetType)
		{
			case AimTargetType::FOV:
				cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
				if (cbFov > bestFov)
					continue;
				break;
		
			case AimTargetType::REAL_DISTANCE :
				distance = pVecTarget.DistTo(cbVecTarget);
				realDistance = GetRealDistanceFOV(distance, Math::CalcAngle(pVecTarget, cbVecTarget), cmd);
				if (realDistance > bestRealDistance)
					continue;
				break;

			default:
				break;
		}
		if (prevDistance == 0 && cbFov != 0)
		{
			prevDistance = cbFov;
			closenstEntity = player;
		}
		else if ( cbFov < prevDistance )
		{
			prevDistance = cbFov;
			closenstEntity = player;
		}
	}
	return closenstEntity;
}
/* 
* This Function Return the Clossest Spot from the Crosshair
* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) 
*/
static Vector GetClosestSpot( CUserCmd* cmd, C_BasePlayer* localPlayer, C_BasePlayer* enemy, AimTargetType aimTargetType = AimTargetType::FOV)
{
	if (!localPlayer || !enemy)
		return Vector(0);

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	float& tempFov = Settings::Legitbot::AutoAim::fov;
	float tempDistance = Settings::Legitbot::AutoAim::fov * 5.f;

	Vector pVecTarget = localPlayer->GetEyePosition();

	Vector tempSpot = Vector(0);

	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(enemy);

	static int len = sizeof(Settings::Legitbot::AutoAim::desiredBones) / sizeof(Settings::Legitbot::AutoAim::desiredBones[0]);
	
	for( int i = 0; i < len; i++ )
	{
		if( !Settings::Legitbot::AutoAim::desiredBones[i] )
			continue;

		int boneID = (*modelType).at(i);
		if( boneID == BONE_INVALID )
			continue;

		Vector bone3D = enemy->GetBonePosition(boneID);

		AutoWall::FireBulletData data;
		float boneDamage = AutoWall::GetDamage(bone3D, !Settings::Legitbot::friendly, data);
		if (boneDamage < Settings::Legitbot::minDamage) 
			continue;

		Vector cbVecTarget = enemy->GetBonePosition(boneID);
		
		if( aimTargetType == AimTargetType::FOV )
		{
			float cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));

			if	( cbFov < tempFov && Entity::IsVisibleThroughEnemies(enemy, boneID) )
			{	
				tempFov = cbFov;	
				tempSpot = cbVecTarget;	
			}
		}
		else if( aimTargetType == AimTargetType::REAL_DISTANCE )
		{
			float cbRealDistance = GetRealDistanceFOV(pVecTarget.DistTo(cbVecTarget), Math::CalcAngle(pVecTarget, cbVecTarget), cmd);

			if	( cbRealDistance < tempDistance && Entity::IsVisibleThroughEnemies(enemy, boneID)  )
			{
				tempDistance = cbRealDistance;
				tempSpot = cbVecTarget;	
			}
		}
	}
	return tempSpot;
}

/*
* Send Clossest Player From Crosshair and and the spot to shoot
*/
static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd, C_BasePlayer* localplayer, bool visibleCheck, Vector* bestSpot, float* bestDamage, AimTargetType aimTargetType = AimTargetType::FOV)
{
	C_BasePlayer *player = GetClosestEnemy(localplayer, cmd); // getting the closest enemy to the crosshair

	if ( !player || !localplayer)
		return nullptr;

	Vector eVecTarget = player->GetBonePosition( (int)Settings::Legitbot::bone );
	auto IsPriorityBoneInFov = IsIzFov( player, cmd, localplayer->GetEyePosition(), eVecTarget );
	
	if ( !IsPriorityBoneInFov )
	{
		Vector tempSpot = GetClosestSpot(cmd, localplayer, player, aimTargetType);
		if( tempSpot.IsZero() || !Entity::IsSpotVisibleThroughEnemies(player, tempSpot) )
			return nullptr;
		eVecTarget = tempSpot;
	}
	
	if (visibleCheck && !Entity::IsSpotVisible(player, eVecTarget))
		return nullptr;
	if ( Settings::Legitbot::SmokeCheck::enabled && LineGoesThroughSmoke( localplayer->GetEyePosition( ), eVecTarget, true ) )
		return nullptr;
	if ( Settings::Legitbot::FlashCheck::enabled && localplayer->IsFlashed() )
		return nullptr;

	*bestSpot = eVecTarget;

	if( bestSpot->IsZero() )
		return nullptr;

	return player;
}

/*
* Recoil controll System
*/
static void RCS(QAngle& angle, C_BasePlayer* player,C_BasePlayer* localplayer, CUserCmd* cmd, bool& shouldAim)
{
	if (!Settings::Legitbot::RCS::enabled || !(cmd->buttons & IN_ATTACK ))
		return;

	if (!Settings::Legitbot::RCS::always_on && !player)
		return;

	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	float aimpunch = cvar->FindVar("weapon_recoil_scale")->GetFloat();
	if ( Settings::Legitbot::silent && aimpunch)
	{
		angle.x -= CurrentPunch.x * Settings::Legitbot::RCS::valueX;
		angle.y -= CurrentPunch.y * Settings::Legitbot::RCS::valueY;
	}
	else if (localplayer->GetShotsFired() > 3 )
	{
		QAngle NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * Settings::Legitbot::RCS::valueX;
		angle.y -= NewPunch.y * Settings::Legitbot::RCS::valueY;
	}
	RCSLastPunch = CurrentPunch;
}

/*
* I don't know What it actuall Does xd
*/
static void AimStep(C_BasePlayer* player, QAngle& angle, CUserCmd* cmd, bool& shouldAim)
{
	if (!Settings::Legitbot::AimStep::enabled)
		return;

	if (!Settings::Legitbot::AutoAim::enabled)
		return;

	if (Settings::Legitbot::Smooth::enabled)
		return;

	if (!shouldAim)
		return;

	if (!Legitbot::aimStepInProgress)
		AimStepLastAngle = cmd->viewangles;

	if (!player)
		return;

	float fov = Math::GetFov(AimStepLastAngle, angle);

	Legitbot::aimStepInProgress = ( fov > (Math::float_rand(Settings::Legitbot::AimStep::min, Settings::Legitbot::AimStep::max)) );

	if (!Legitbot::aimStepInProgress)
		return;

    cmd->buttons &= ~(IN_ATTACK); // aimstep in progress, don't shoot.

	QAngle deltaAngle = AimStepLastAngle - angle;

	Math::NormalizeAngles(deltaAngle);
	float randX = Math::float_rand(Settings::Legitbot::AimStep::min, std::min(Settings::Legitbot::AimStep::max, fov));
	float randY = Math::float_rand(Settings::Legitbot::AimStep::min, std::min(Settings::Legitbot::AimStep::max, fov));
	if (deltaAngle.y < 0)
		AimStepLastAngle.y += randY;
	else
		AimStepLastAngle.y -= randY;

	if(deltaAngle.x < 0)
		AimStepLastAngle.x += randX;
	else
		AimStepLastAngle.x -= randX;

	angle = AimStepLastAngle;
}

static void Salt(float& smooth)
{
	float sine = sin (globalVars->tickcount);
	float salt = sine * Settings::Legitbot::Smooth::Salting::multiplier;
	float oval = smooth + salt;
	smooth *= oval;
}

/*
 * Add smoothness to the aim but bit more soothness is bad for helth xd
 */
static void Smooth(C_BasePlayer* player, QAngle& angle, bool& shouldAim)
{
	if (!Settings::Legitbot::Smooth::enabled)
		return;
	if (!shouldAim || !player)
		return;
	if (Settings::Legitbot::silent)
		return;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	QAngle delta = angle - viewAngles;
	Math::NormalizeAngles(delta);

	float smooth = powf(Settings::Legitbot::Smooth::value, 0.4f); // Makes more slider space for actual useful values

	smooth = std::min(0.99f, smooth);

	if (Settings::Legitbot::Smooth::Salting::enabled)
		Salt(smooth);

	QAngle toChange = {0,0,0};

	SmoothType type = Settings::Legitbot::Smooth::type;

	if (type == SmoothType::SLOW_END)
		toChange = delta - (delta * smooth);
	else if (type == SmoothType::CONSTANT || type == SmoothType::FAST_END)
	{
		float coeff = (1.0f - smooth) / delta.Length() * 4.f;

		if (type == SmoothType::FAST_END)
			coeff = powf(coeff, 2.f) * 10.f;

		coeff = std::min(1.f, coeff);
		toChange = delta * coeff;
	}

	angle = viewAngles + toChange;
}

/*
 * Hitchance Source from NanoScence
 */
static bool hitchance(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
	float hitchance = 101;
	activeWeapon->UpdateAccuracyPenalty();
	if (activeWeapon)
	{
		float inaccuracy = activeWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;
		
		return hitchance >= (Settings::Legitbot::ShootAssist::Hitchance::value * 1.5);
	}
	return true;
}

static void AutoCrouch(C_BasePlayer* player, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoCrouch::enabled)
		return;

	if (!player)
		return;

	cmd->buttons |= IN_BULLRUSH | IN_DUCK;
}

static void AutoSlow(C_BasePlayer* player,C_BasePlayer* localplayer, float& forward, float& sideMove, float& bestDamage, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoSlow::enabled || !player || activeWeapon->GetNextPrimaryAttack() > globalVars->curtime || !activeWeapon || activeWeapon->GetAmmo() == 0)
		return;

	// if( Settings::Legitbot::ShootAssist::Hitchance::enabled && !hitchance(localplayer, activeWeapon))
	// {
    //     forward = 0;
	// 	sideMove = 0;
	// }
    if ( (activeWeapon->GetSpread() + activeWeapon->GetInaccuracy()) == (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f) ) 
    {
        cmd->buttons |= IN_WALK;
		forward = 0;
		sideMove = 0;
		cmd->upmove = 0;
    }
}

static void AutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoPistol::enabled)
		return;
	if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;
	if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
		return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
        cmd->buttons &= ~IN_ATTACK;
}

/*
* Add Fake Mouse movement because vacnet is a bitch
*/
static void FixMouseDeltas(CUserCmd* cmd, const QAngle &angle, const QAngle &oldAngle, bool &shouldAim)
{
    if( !shouldAim)
        return;
    QAngle delta = angle - oldAngle;
    float sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
    float m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
    float m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
    float zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

    Math::NormalizeAngles(delta);

    cmd->mousedx = -delta.y / ( m_yaw * sens * zoomMultiplier );
    cmd->mousedy = delta.x / ( m_pitch * sens * zoomMultiplier );
}

bool AimKeyOnly (CUserCmd* cmd)
{
	if (cmd->buttons & IN_ATTACK)
        return true;
    else if (inputSystem->IsButtonDown(Settings::Legitbot::aimkey))
        return true;
    else
        return false;
}

static void AutoShoot(C_BasePlayer* localplayer, C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, bool& shouldAim)
{
    if ( activeWeapon->GetAmmo() == 0 || !shouldAim )
		return;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

    if (Settings::Legitbot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2))
    {
		cmd->buttons |= IN_ATTACK2;
		return; // continue next tick
    }

	//cvar->ConsoleDPrintf(XORSTR("WE are now applying auto shoot"));
    float nextPrimaryAttack = activeWeapon->GetNextPrimaryAttack();

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
	{	
		if (nextPrimaryAttack > globalVars->curtime)
	    	cmd->buttons &= ~IN_ATTACK;
		else
	    	cmd->buttons |= IN_ATTACK;
	}
}

static bool CanShoot(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeweapon)
{
	if (!localplayer)
		return false;
	if (!localplayer->GetAlive())
		return false;
	if (!activeweapon)
		return false;
	if (!activeweapon->GetInReload())
		return false;

	return true;
}
void Legitbot::CreateMove(CUserCmd* cmd)
{

	if(!Settings::Legitbot::enabled)
		return; 		
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

	if (!CanShoot(localplayer, activeWeapon))
		return;

	if (Legitbot::prevWeapon != (ItemDefinitionIndex)*activeWeapon->GetItemDefinitionIndex())
	{
		Legitbot::prevWeapon = (ItemDefinitionIndex)*activeWeapon->GetItemDefinitionIndex();
		Legitbot::UpdateValues();
	}

	QAngle oldAngle;
	engine->GetViewAngles(oldAngle);
	
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;
	bool shouldAim = false;
	QAngle angle = cmd->viewangles;
	static QAngle lastRandom = QAngle(0);
	
	Vector localEye = localplayer->GetEyePosition();

	if (Settings::Legitbot::IgnoreJump::enabled && (!(localplayer->GetFlags() & FL_ONGROUND) && localplayer->GetMoveType() != MOVETYPE_LADDER))
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

    Vector bestSpot = Vector(0);
	float bestDamage = float(0);

	C_BasePlayer* player = GetClosestPlayerAndSpot(cmd, localplayer, !Settings::Legitbot::AutoWall::enabled, &bestSpot, &bestDamage);

	if (player)
	{
         // Conditions if AimKeyOnly enabled
        if (Settings::Legitbot::aimkeyOnly)
			shouldAim = AimKeyOnly(cmd);
		else if( (cmd->buttons & IN_ATTACK) || Settings::Triggerbot::Magnet::enabled)
			shouldAim = true;

		Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.

		if (shouldAim)
		{	
			if (Settings::Legitbot::Prediction::enabled)
				VelocityExtrapolate(localplayer, bestSpot, localEye); // get eye pos next tick

			angle = Math::CalcAngle(localEye, bestSpot);

			if (Settings::Legitbot::ErrorMargin::enabled)
			{
				static int lastShotFired = 0;
				if ((localplayer->GetShotsFired() > lastShotFired)) //get new random spot when firing a shot or when aiming at a new target
					lastRandom = ApplyErrorToAngle(&angle, Settings::Legitbot::ErrorMargin::value);

				angle += lastRandom;
				lastShotFired = localplayer->GetShotsFired();
			}
		}
	}
	else // No player to Shoot
	{
        Settings::Debug::AutoAim::target = Vector(0);
        lastRandom = QAngle(0);	
    }

	AimStep(player, angle, cmd, shouldAim);
	AutoCrouch(player, cmd);
	AutoShoot(localplayer, player, activeWeapon, cmd, shouldAim);
	AutoSlow(player,localplayer, oldForward, oldSideMove, bestDamage, activeWeapon, cmd);
	AutoPistol(activeWeapon, cmd);
	RCS(angle, player,localplayer, cmd, shouldAim);
	Smooth(player, angle, shouldAim);
	
    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

	FixMouseDeltas(cmd, angle, oldAngle, shouldAim);
	
	cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);

	if( !Settings::Legitbot::silent )
    	engine->SetViewAngles(cmd->viewangles);

}

void Legitbot::FireGameEvent(IGameEvent* event)
{
	if(!Settings::Legitbot::enabled || !event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0 )
	{
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
			return;
		Legitbot::friends.clear();
	}
	if( strcmp(event->GetName(), XORSTR("player_death")) == 0 )
	{
		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) // suicide
			return;

		if (attacker_id != engine->GetLocalPlayer())
			return;

		killTimes.push_back(Util::GetEpochTime());
	}
}

void Legitbot::UpdateValues()
{
	if (!engine->IsInGame() || !Settings::Legitbot::enabled)
		return;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	
	if (!localplayer)
		return;
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon)
		return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
		index = *activeWeapon->GetItemDefinitionIndex();

	const AimbotWeapon_t& currentWeaponSetting = Settings::Legitbot::weapons.at(index);

	Settings::Legitbot::silent = currentWeaponSetting.silent;
	Settings::Legitbot::friendly = currentWeaponSetting.friendly;
	Settings::Legitbot::bone = currentWeaponSetting.bone;
	Settings::Legitbot::aimkey = currentWeaponSetting.aimkey;
	Settings::Legitbot::aimkeyOnly = currentWeaponSetting.aimkeyOnly;
	Settings::Legitbot::Smooth::enabled = currentWeaponSetting.smoothEnabled;
	Settings::Legitbot::Smooth::value = currentWeaponSetting.smoothAmount;
	Settings::Legitbot::Smooth::type = currentWeaponSetting.smoothType;
	Settings::Legitbot::ErrorMargin::enabled = currentWeaponSetting.errorMarginEnabled;
	Settings::Legitbot::ErrorMargin::value = currentWeaponSetting.errorMarginValue;
	Settings::Legitbot::AutoAim::enabled = currentWeaponSetting.autoAimEnabled;
	Settings::Legitbot::ShootAssist::enabled = currentWeaponSetting.shootassist;
	Settings::Legitbot::AutoAim::fov = currentWeaponSetting.LegitautoAimFov;
	Settings::Legitbot::AutoAim::closestBone = currentWeaponSetting.closestBone;
	Settings::Legitbot::AutoAim::engageLock = currentWeaponSetting.engageLock;
	Settings::Legitbot::AutoAim::engageLockTR = currentWeaponSetting.engageLockTR;
	Settings::Legitbot::AutoAim::engageLockTTR = currentWeaponSetting.engageLockTTR;
	Settings::Legitbot::AimStep::enabled = currentWeaponSetting.aimStepEnabled;
	Settings::Legitbot::AimStep::min = currentWeaponSetting.aimStepMin;
	Settings::Legitbot::AimStep::max = currentWeaponSetting.aimStepMax;
	Settings::Legitbot::AutoPistol::enabled = currentWeaponSetting.autoPistolEnabled;
	Settings::Legitbot::AutoShoot::autoscope = currentWeaponSetting.autoScopeEnabled;
	Settings::Legitbot::RCS::enabled = currentWeaponSetting.rcsEnabled;
	Settings::Legitbot::RCS::always_on = currentWeaponSetting.rcsAlwaysOn;
	Settings::Legitbot::RCS::valueX = currentWeaponSetting.rcsAmountX;
	Settings::Legitbot::RCS::valueY = currentWeaponSetting.rcsAmountY;
	Settings::Legitbot::NoShoot::enabled = currentWeaponSetting.noShootEnabled;
	Settings::Legitbot::IgnoreJump::enabled = currentWeaponSetting.ignoreJumpEnabled;
	Settings::Legitbot::IgnoreEnemyJump::enabled = currentWeaponSetting.ignoreEnemyJumpEnabled;
	Settings::Legitbot::Smooth::Salting::enabled = currentWeaponSetting.smoothSaltEnabled;
	Settings::Legitbot::Smooth::Salting::multiplier = currentWeaponSetting.smoothSaltMultiplier;
	Settings::Legitbot::SmokeCheck::enabled = currentWeaponSetting.smokeCheck;
	Settings::Legitbot::FlashCheck::enabled = currentWeaponSetting.flashCheck;
	Settings::Legitbot::ShootAssist::Hitchance::enabled = currentWeaponSetting.hitchanceEnaled;
	Settings::Legitbot::ShootAssist::Hitchance::value = currentWeaponSetting.hitchance;
	Settings::Legitbot::ShootAssist::ShotDelay::Value = currentWeaponSetting.shotDelay;
	Settings::Legitbot::ShootAssist::MinShotFire::value = currentWeaponSetting.minShotFire;
	Settings::Legitbot::AutoWall::enabled = currentWeaponSetting.autoWallEnabled;
	Settings::Legitbot::minDamage = currentWeaponSetting.MinDamage;
	Settings::Legitbot::AutoSlow::enabled = currentWeaponSetting.autoSlow;
	Settings::Legitbot::ScopeControl::enabled = currentWeaponSetting.scopeControlEnabled;
	Settings::Triggerbot::enabled = currentWeaponSetting.TriggerBot;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		Settings::Legitbot::AutoAim::desiredBones[bone] = currentWeaponSetting.desiredBones[bone];

	Settings::Legitbot::AutoAim::realDistance = currentWeaponSetting.autoAimRealDistance;
}
