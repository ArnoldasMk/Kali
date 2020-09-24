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


#include "legitbot.h"


bool Legitbot::aimStepInProgress = false;
std::vector<int64_t> Legitbot::friends = { };
std::vector<long> killTimes = { 0 }; // the Epoch time from when we kill someone


QAngle AimStepLastAngle;
QAngle LastPunch;
static bool shouldAim = false;
int Legitbot::targetAimbot = -1;

QAngle ApplyErrorToAngle(QAngle* angles, float margin)
{
	QAngle error;
	error.Random(-1.0f, 1.0f);
	error *= margin;
	angles->operator+=(error);
	return error;
}

void VelocityExtrapolate(C_BasePlayer* player, Vector& aimPos, Vector& localeye)
{
	if (!player || !player->GetAlive())
		return;
	aimPos += (player->GetVelocity() * globalVars->interval_per_tick);
	localeye += (player->GetVelocity() * globalVars->interval_per_tick);
}

bool IsInFov(C_BasePlayer* localplayer, C_BasePlayer* player, const Vector &spot, const LegitWeapon_t& weaponSettings)
{
	if (!localplayer || !localplayer->GetAlive()) return false; 
	if (!player || !player->GetAlive())	return false;

	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

	float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, spot) );
	if (cbFov > weaponSettings.LegitautoAimFov)
		return false;
	
	return true;
}

void GetClosestSpot(C_BasePlayer* localplayer, C_BasePlayer* enemy, Vector &BestSpot, const LegitWeapon_t& currentSettings)
{
	if (!localplayer || !localplayer->GetAlive())
		return;
	if (!enemy || !enemy->GetAlive())
		return;

	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(enemy);

	float bestFov = currentSettings.LegitautoAimFov;

	int len = 31;
	if (currentSettings.mindamage && currentSettings.minDamagevalue >= 70)
		len = BONE_HEAD;
	else if (currentSettings.mindamage && currentSettings.minDamagevalue <= 70)
		len = BONE_RIGHT_ELBOW;

	int PrevDamage = 0;

	for( int i = 0; i < len; i++ )
	{
		int boneID = (*modelType).at(i);

		if( boneID == BONE_INVALID )
			continue;

		Vector bone3D = enemy->GetBonePosition( boneID );

		if (!currentSettings.mindamage)
		{
			Vector pVecTarget = localplayer->GetEyePosition();
			QAngle viewAngles;
				engine->GetViewAngles(viewAngles);

			Vector cbVecTarget = bone3D;
			float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
			
			if (cbFov < bestFov)
			{
				bestFov = cbFov;
				BestSpot = bone3D;
			}
			continue;
		}
		
		if (!IsInFov(localplayer, enemy, bone3D, currentSettings))	continue;

		int boneDamage = AutoWall::GetDamage(bone3D, true);

		if (boneDamage >= enemy->GetHealth())
			BestSpot = bone3D;
		else if ( (boneDamage > PrevDamage && boneDamage >= currentSettings.minDamagevalue) )
		{	
			BestSpot = bone3D;
			PrevDamage = boneDamage;
		}
		
		if (boneDamage >= 70 && i >= BONE_HEAD)
			break;
	}
}

void RCS(QAngle& angle, C_BasePlayer* player, CUserCmd* cmd, bool shouldAim, const LegitWeapon_t& currentSettings)
{
	if (!currentSettings.rcsEnabled)	return;
	if ( !(cmd->buttons&IN_ATTACK) )	return;

	bool hastarget = currentSettings.rcsAlwaysOn && shouldAim && player;

	if (!currentSettings.rcsAlwaysOn && !hastarget)		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	// float aimpunch = cvar->FindVar("weapon_recoil_scale")->GetFloat();

	if ( currentSettings.silent)
	{
		angle.x -= CurrentPunch.x * currentSettings.rcsAmountX;
		angle.y -= CurrentPunch.y * currentSettings.rcsAmountY;
		return;
	}
	else if (localplayer->GetShotsFired() > 1)
	{
		QAngle newAngle = CurrentPunch-LastPunch;
		angle.x -= (newAngle.x * currentSettings.rcsAmountX);
		angle.y -= (newAngle.y * currentSettings.rcsAmountY);
	}

	LastPunch.x = CurrentPunch.x;
	LastPunch.y = CurrentPunch.y;
}

void AimStep(C_BasePlayer* player, QAngle& angle, CUserCmd* cmd, bool& shouldAim, const LegitWeapon_t& currentSettings)
{
	if (!currentSettings.aimStepEnabled)
		return;

	if (!currentSettings.autoAimEnabled)
		return;

	if (currentSettings.smoothEnabled)
		return;

	if (!shouldAim)
		return;

	if (!Legitbot::aimStepInProgress)
		AimStepLastAngle = cmd->viewangles;

	if (!player || !player->GetAlive())
		return;

	float fov = Math::GetFov(AimStepLastAngle, angle);

	Legitbot::aimStepInProgress = ( fov > (Math::float_rand(currentSettings.aimStepMin, currentSettings.aimStepMax)) );

	if (!Legitbot::aimStepInProgress)
		return;

    cmd->buttons &= ~(IN_ATTACK); // aimstep in progress, don't shoot.

	QAngle deltaAngle = AimStepLastAngle - angle;

	Math::NormalizeAngles(deltaAngle);
	float randX = Math::float_rand(currentSettings.aimStepMin, std::min(currentSettings.aimStepMax, fov));
	float randY = Math::float_rand(currentSettings.aimStepMin, std::min(currentSettings.aimStepMax, fov));
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

void Salt(float& smooth, const LegitWeapon_t& currentSettings)
{
	float sine = sin (globalVars->tickcount);
	float salt = sine * currentSettings.smoothSaltMultiplier;
	float oval = smooth + salt;
	smooth *= oval;
}

bool AutoSlow(C_BasePlayer* player,C_BasePlayer* localplayer, float& forward, float& sideMove, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, const LegitWeapon_t& currentSettings)
{
	if (!currentSettings.autoSlow )
		return true;
	if (!player || !player->GetAlive()) 
		return false;
	if (!localplayer || !localplayer->GetAlive())
		return false;
	if ( !activeWeapon || activeWeapon->GetInReload())
		return false;

	if (currentSettings.autoScopeEnabled && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) && !(cmd->buttons&IN_ATTACK) )
	{
		cmd->buttons |= IN_ATTACK2;
		return false;
	}

	if (currentSettings.hitchanceEnaled )
	{
		cvar->ConsoleDPrintf(XORSTR("slowing Down"));
		cmd->buttons |= IN_WALK;
		forward = -forward;
		sideMove = -sideMove;
		cmd->upmove = 0;
	}
    else if ( (activeWeapon->GetSpread() + activeWeapon->GetInaccuracy()) == (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f) ) 
    {
        cmd->buttons |= IN_WALK;
		forward = 0;
		sideMove = 0;
		cmd->upmove = 0;
    }
	cvar->ConsoleDPrintf(XORSTR("In AutoSlow Ending"));
	return false;
}

bool canShoot(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, const LegitWeapon_t& currentSettings)
{
	if(!localplayer || !localplayer->GetAlive() )
		return false;
	if (!activeWeapon || activeWeapon->GetInReload())
		return false;
	if (!currentSettings.hitchanceEnaled)
	{
		if ( (activeWeapon->GetSpread() + activeWeapon->GetInaccuracy()) <= (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f) )
			return true;
		else
			return false;
	}
	
	activeWeapon->UpdateAccuracyPenalty();
	float hitchance = activeWeapon->GetInaccuracy();
	// hitchance = activeWeapon->GetInaccuracy();
	if (hitchance == 0) hitchance = 0.0000001;
	hitchance = 1/(hitchance);
	
	return hitchance >= (currentSettings.hitchance*2);
}

void AutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, const LegitWeapon_t& currentSettings)
{
    if (!currentSettings.autoShoot)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if (!player)
		return;
	if (!activeWeapon || activeWeapon->GetInReload())
		return;
	
	if (cmd->buttons & IN_USE)
		return;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		return;
		
	if ( canShoot(localplayer, activeWeapon, currentSettings) )
	{
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else
			cmd->buttons |= IN_ATTACK;	
		return;
	}	

	AutoSlow(player,localplayer, forrwordMove, sideMove, activeWeapon, cmd, currentSettings);
}

void Smooth(C_BasePlayer* player, QAngle& angle, bool& shouldAim, const LegitWeapon_t& currentSettings)
{
	if (!currentSettings.smoothEnabled)
		return;
	if (!shouldAim || !player || !player->GetAlive())
		return;

	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

	QAngle delta = angle - viewAngles;
	Math::NormalizeAngles(delta);

	float smooth = powf(currentSettings.smoothAmount, 0.4f); // Makes more slider space for actual useful values

	smooth = std::min(0.99f, smooth);

	if (currentSettings.smoothSaltEnabled)
		Salt(smooth, currentSettings);

	QAngle toChange = {0,0,0};

	SmoothType type = currentSettings.smoothType;

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

void AutoCrouch(C_BasePlayer* player, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoCrouch::enabled)
		return;

	if (!player || !player->GetAlive())
		return;

	cmd->buttons |= IN_BULLRUSH | IN_DUCK;
}

void AutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, const LegitWeapon_t& currentSettings)
{
	if (!activeWeapon || activeWeapon->GetInReload())
		return;
	if (!currentSettings.autoPistolEnabled)
		return;
	if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;
	if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
		return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
        cmd->buttons &= ~IN_ATTACK;
}

void FixMouseDeltas(CUserCmd* cmd, const QAngle &angle, const QAngle &oldAngle, bool &shouldAim)
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

C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer, const LegitWeapon_t& currentSettings)
{
	if (!localplayer || !localplayer->GetAlive())
		return nullptr;

	float bestFov = currentSettings.LegitautoAimFov;

	Vector pVecTarget = localplayer->GetEyePosition();
	C_BasePlayer* enemy = nullptr;
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

	int maxClient = engine->GetMaxClients();
	for (int i = maxClient; i > 1 ; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*)entityList->GetClientEntity(i);

		if (!player
	    	|| player == localplayer
	    	|| player->GetDormant()
	    	|| !player->GetAlive()
	    	|| player->GetImmune())
	    	continue;

		if (Entity::IsTeamMate(player, localplayer))
	   	 	continue;

		Vector cbVecTarget = player->GetEyePosition();
		
		float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
		if (cbFov > bestFov)
			continue;
	
		bestFov = cbFov;
		enemy = player;
	}

	return enemy;
}

C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd, C_BasePlayer* localplayer, bool visibleCheck, Vector& bestSpot, float& bestDamage, const LegitWeapon_t& currentSettings)
{
	C_BasePlayer *player = nullptr;
	player = GetClosestEnemy(localplayer, currentSettings); // getting the closest enemy to the crosshair

	if (!localplayer || !localplayer->GetAlive() )	return nullptr;
	if ( !player || !player->GetAlive())	return nullptr;
	if ( localplayer->IsFlashed() )	return nullptr;

	int BoneId = (int)(currentSettings.bone);
	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(player);
	BoneId = (*modelType).at(BoneId);
	Vector bone3d = player->GetBonePosition( BoneId );

	bool IsPriorityBoneInFov = IsInFov( localplayer, player, bone3d  , currentSettings);
	
	if ( !IsPriorityBoneInFov )
		GetClosestSpot(localplayer, player, bone3d, currentSettings);
	
	if ( LineGoesThroughSmoke( localplayer->GetEyePosition( ), bone3d, true ) )
		return nullptr;

	bestSpot = bone3d;

	if (AutoWall::GetDamage(bone3d, true) <= 0 || !Entity::IsSpotVisible(player, bone3d))
		return nullptr;

	return player;
}

void Legitbot::CreateMove(CUserCmd* cmd)
{
	if(!Settings::Legitbot::enabled)
		return;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetInReload())
		return;
	// UpdateValues();

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
		index = *activeWeapon->GetItemDefinitionIndex();
	const LegitWeapon_t& currentWeaponSetting = Settings::Legitbot::weapons.at(index);

	QAngle oldAngle;
		engine->GetViewAngles(oldAngle);

	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;
	shouldAim = false;

	QAngle angle = cmd->viewangles;
	static QAngle lastRandom = QAngle(0);
	Vector localEye = localplayer->GetEyePosition();

	if (currentWeaponSetting.ignoreJumpEnabled && (!(localplayer->GetFlags() & FL_ONGROUND) && localplayer->GetMoveType() != MOVETYPE_LADDER))
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

    Vector bestSpot = Vector(0);
	float bestDamage = float(0);

	C_BasePlayer* player = GetClosestPlayerAndSpot(cmd, localplayer, true, bestSpot, bestDamage, currentWeaponSetting);

	if (player)
	{
		// cvar->ConsoleDPrintf(XORSTR("Player found \n"));
		if (!currentWeaponSetting.aimkeyOnly && ( cmd->buttons&IN_ATTACK || currentWeaponSetting.autoShoot))
			shouldAim = true; 
		else if (inputSystem->IsButtonDown(currentWeaponSetting.aimkey))
			shouldAim = true;
			
		Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.

		if (shouldAim)
		{	
			if (currentWeaponSetting.predEnabled)
				VelocityExtrapolate(player, bestSpot, localEye); // get eye pos next tick

			angle = Math::CalcAngle(localEye, bestSpot);

			if (currentWeaponSetting.errorMarginEnabled)
			{
				static int lastShotFired = 0;
				if ((localplayer->GetShotsFired() > lastShotFired)) //get new random spot when firing a shot or when aiming at a new target
					lastRandom = ApplyErrorToAngle(&angle, currentWeaponSetting.errorMarginValue);

				angle += lastRandom;
				lastShotFired = localplayer->GetShotsFired();
			}
		}

		
		AutoCrouch(player, cmd);
		AutoPistol(activeWeapon, cmd, currentWeaponSetting);
	}
	else // No player to Shoot
	{
        Settings::Debug::AutoAim::target = Vector(0);	
		lastRandom = QAngle(0);
    }

	AutoShoot(player, localplayer, activeWeapon, cmd, oldForward, oldSideMove, currentWeaponSetting);
	Smooth(player, angle, shouldAim, currentWeaponSetting);
	RCS(angle, player, cmd, shouldAim, currentWeaponSetting);
	AimStep(player, angle, cmd, shouldAim, currentWeaponSetting);
	
    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

	FixMouseDeltas(cmd, angle, oldAngle, shouldAim);
	
	cmd->viewangles = angle;

	Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);

	if( !currentWeaponSetting.silent )
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

