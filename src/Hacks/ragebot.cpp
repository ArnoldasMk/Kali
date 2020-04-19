#include "ragebot.h"
#include "autowall.h"

#include "../Utils/xorstring.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../Utils/bonemaps.h"
#include "../settings.h"
#include "../interfaces.h"

std::vector<int64_t> Ragebot::friends = { };
std::vector<long> RagebotkillTimes = { 0 }; // the Epoch time from when we kill someone

bool RagebotShouldAim;
int prevWeaponIndex = 0;
static QAngle RagebotRCSLastPunch = {0,0,0};

int Ragebot::targetAimbot = -1;
const int headVectors = 11;

/* Fills points Vector. True if successful. False if not.  Credits for Original method - ReactiioN */
static bool HeadMultiPoint(C_BasePlayer *player, Vector points[])
{
	matrix3x4_t matrix[128];

	if( !player->SetupBones(matrix, 128, 0x100, 0.f) )
		return false;
	model_t *pModel = player->GetModel();
	if( !pModel )
		return false;

	studiohdr_t *hdr = modelInfo->GetStudioModel(pModel);
	if( !hdr )
		return false;
	mstudiobbox_t *bbox = hdr->pHitbox((int)Hitbox::HITBOX_HEAD, 0);
	if( !bbox )
		return false;

	Vector mins, maxs;
	Math::VectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
	Math::VectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);

	Vector center = ( mins + maxs ) * 0.5f;
	// 0 - center, 1 - forehead, 2 - skullcap, 3 - upperleftear, 4 - upperrightear, 5 - uppernose, 6 - upperbackofhead
	// 7 - leftear, 8 - rightear, 9 - nose, 10 - backofhead
	for( int i = 0; i < headVectors; i++ ) // set all points initially to center mass of head.
		points[i] = center;
	points[1].z += bbox->radius * 0.60f; // morph each point.
	points[2].z += bbox->radius * 1.25f; // ...
	points[3].x += bbox->radius * 0.80f;
	points[3].z += bbox->radius * 0.60f;
	points[4].x -= bbox->radius * 0.80f;
	points[4].z += bbox->radius * 0.90f;
	points[5].y += bbox->radius * 0.80f;
	points[5].z += bbox->radius * 0.90f;
	points[6].y -= bbox->radius * 0.80f;
	points[6].z += bbox->radius * 0.90f;
	points[7].x += bbox->radius * 0.80f;
	points[8].x -= bbox->radius * 0.80f;
	points[9].y += bbox->radius * 0.80f;
	points[10].y -= bbox->radius * 0.80f;

	return true;
}
static float AutoWallBestSpot(C_BasePlayer *player, Vector &bestSpot)
{
	float bestDamage = Settings::Ragebot::AutoWall::value;
	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(player);

	static int len = sizeof(Settings::Ragebot::AutoAim::desiredBones) / sizeof(Settings::Ragebot::AutoAim::desiredBones[0]);

	for( int i = 0; i < len; i++ )
	{
		if( !Settings::Ragebot::AutoAim::desiredBones[i] )
			continue;
		if( i == BONE_HEAD ) // head multipoint
		{
			Vector headPoints[headVectors];
			if( !HeadMultiPoint(player, headPoints) )
				continue;
			for( int j = 0; j < headVectors; j++ )
			{
				Autowall::FireBulletData data;
				float spotDamage = Autowall::GetDamage(headPoints[j], !Settings::Ragebot::friendly, data);
				if( spotDamage >= bestDamage )
				{
					bestSpot = headPoints[j];
					if( spotDamage > player->GetHealth() )
						return spotDamage;
					bestDamage = spotDamage;
				}
			}
		}
		int boneID = (*modelType).at(i);
		if( boneID == BONE_INVALID ) // bone not available on this modeltype.
			continue;

		Vector bone3D = player->GetBonePosition(boneID);

		Autowall::FireBulletData data;
		float boneDamage = Autowall::GetDamage(bone3D, !Settings::Ragebot::friendly, data);
		if( boneDamage >= bestDamage )
		{
			bestSpot = bone3D;
			if( boneDamage > player->GetHealth() )
				return boneDamage;

			bestDamage = boneDamage;
		}
	}
	return bestDamage;
}

static Vector VelocityExtrapolate(C_BasePlayer* player, Vector aimPos)
{
	return aimPos + (player->GetVelocity() * globalVars->interval_per_tick);
}

/* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) */
static float GetClosestDamage(CUserCmd* cmd, C_BasePlayer* localPlayer, C_BasePlayer* enemy)
{
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	float bestDamage = 0.1f;

	float tempFov = Settings::Ragebot::AutoAim::fov;

	Vector pVecTarget = localPlayer->GetEyePosition();

	Vector tempSpot = {0,0,0};

	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(enemy);

	static int len = sizeof(Settings::Ragebot::AutoAim::desiredBones) / sizeof(Settings::Ragebot::AutoAim::desiredBones[0]);
	for( int i = 0; i < len; i++ )
	{
		if( !Settings::Ragebot::AutoAim::desiredBones[i] )
			continue;

		int boneID = (*modelType).at(i);
		if( boneID == BONE_INVALID )
			continue;
		
		Vector cbVecTarget = enemy->GetBonePosition(boneID);
			float cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));

			if( cbFov < tempFov )
			{
				if( Entity::IsVisibleThroughEnemies(enemy, boneID) )
				{
					tempFov = cbFov;
					tempSpot = cbVecTarget;
				}
			}
		Autowall::FireBulletData dataVisible;
		Vector bone3D = enemy->GetBonePosition(boneID);
		bestDamage = Autowall::GetDamage(bone3D, !Settings::Ragebot::friendly, dataVisible);
	}
	
	
	return bestDamage;
}

static Vector GetClosestSpot( CUserCmd* cmd, C_BasePlayer* localPlayer, C_BasePlayer* enemy, AimTargetType aimTargetType = AimTargetType::FOV)
{
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	float tempFov = Settings::Ragebot::AutoAim::fov;
	// float tempDistance = Settings::Ragebot::AutoAim::fov * 5.f;

	Vector pVecTarget = localPlayer->GetEyePosition();

	Vector tempSpot = {0,0,0};

	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(enemy);

	static int len = sizeof(Settings::Ragebot::AutoAim::desiredBones) / sizeof(Settings::Ragebot::AutoAim::desiredBones[0]);
	for( int i = 0; i < len; i++ )
	{
		if( !Settings::Ragebot::AutoAim::desiredBones[i] )
			continue;

		int boneID = (*modelType).at(i);
		if( boneID == BONE_INVALID )
			continue;

		Vector cbVecTarget = enemy->GetBonePosition(boneID);

		if( aimTargetType == AimTargetType::FOV )
		{
			float cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));

			if( cbFov < tempFov )
			{
				if( Entity::IsVisibleThroughEnemies(enemy, boneID) )
				{
					tempFov = cbFov;
					tempSpot = cbVecTarget;
				}
			}
		}
	}
	return tempSpot;
}

static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd, Vector* bestSpot, float* bestDamage, AimTargetType aimTargetType = AimTargetType::FOV)
{
	static C_BasePlayer* lockedOn = nullptr;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BasePlayer* closestEntity = nullptr;

	float bestFov = Settings::Ragebot::AutoAim::fov;

	// int maxClientEntity = engine->GetMaxClients();
	// if( maxClientEntity > 5) { maxClientEntity = 5; }

	for (int i = 1; i < engine->GetMaxClients(); ++i)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player
			|| player == localplayer
			|| player->GetDormant()
			|| !player->GetAlive()
			|| player->GetImmune())
			continue;

		if (!Settings::Ragebot::friendly && Entity::IsTeamMate(player, localplayer))
			continue;

		if( !Ragebot::friends.empty() ) // check for friends, if any
		{
			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			if (std::find(Ragebot::friends.begin(), Ragebot::friends.end(), entityInformation.xuid) != Ragebot::friends.end())
				continue;
		}

		Ragebot::targetAimbot = i;

		float damage = GetClosestDamage(cmd, localplayer, player);
		Vector tempSpot = GetClosestSpot(cmd, localplayer, player);
		if( tempSpot.IsZero() || !Entity::IsSpotVisibleThroughEnemies(player, tempSpot) )
			continue;
		Vector eVecTarget = tempSpot;

		Vector pVecTarget = localplayer->GetEyePosition();
        lastRayStart = pVecTarget;
        lastRayEnd = eVecTarget;


		float distance = pVecTarget.DistTo(eVecTarget);

		QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

		float fov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, eVecTarget));

		if (fov > bestFov)
			continue;

		Vector wallBangSpot = {0,0,0};
		float damageAutoWall = AutoWallBestSpot(player, wallBangSpot); // sets Vector Angle, returns damage of hitting that spot.

		if(damage > damageAutoWall >= Settings::Ragebot::AutoWall::value && ( damageAutoWall > 0.f ))
		{
			closestEntity = player;
			*bestSpot = eVecTarget;
			bestFov = fov;
			return closestEntity;
		}
		else if (damageAutoWall >= Settings::Ragebot::AutoWall::value && ( damageAutoWall > 0.f ))
		{
			*bestDamage = damageAutoWall;
			*bestSpot = wallBangSpot;
			closestEntity = player;
			lastRayEnd = wallBangSpot;
			return closestEntity;
		}
		else
			return nullptr;
	}

	// lockedOn = closestEntity;
	// if( bestSpot->IsZero() )
	// 	return nullptr;

	/*
	if( closestEntity )
	{
		IEngineClient::player_info_t playerInfo;
		engine->GetPlayerInfo(closestEntity->GetIndex(), &playerInfo);
		cvar->ConsoleDPrintf("%s is Closest.\n", playerInfo.name);
	}
	*/

	return closestEntity;
}


//Hitchance
float Ragebothitchance()
{
	float hitchance = 101;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (activeWeapon)
	{
		if (Settings::Ragebot::HitChance::value > 0)
		{
			float inaccuracy = activeWeapon->GetInaccuracy();
			if (inaccuracy == 0) inaccuracy = 0.0000001;
			inaccuracy = 1 / inaccuracy;
			hitchance = inaccuracy;
		}
		return hitchance;
	}
}

static void RagebotRCS(QAngle& angle, C_BasePlayer* player, CUserCmd* cmd, C_BasePlayer* localplayer)
{

	if (!(cmd->buttons & IN_ATTACK))
		return;

	bool hasTarget = RagebotShouldAim && player;

	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	if ( Settings::Ragebot::silent || hasTarget )
	{
		angle.x -= CurrentPunch.x * 2.0f;
		angle.y -= CurrentPunch.y * 2.0f;
	}
	else if (localplayer->GetShotsFired() > 1)
	{
		QAngle NewPunch = { CurrentPunch.x - RagebotRCSLastPunch.x, CurrentPunch.y - RagebotRCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * 2.0f;
		angle.y -= NewPunch.y * 2.0f;
	}

	RagebotRCSLastPunch = CurrentPunch;
}

static void RagebotAutoCrouch(C_BasePlayer* player, CUserCmd* cmd)
{
	if (!Settings::Ragebot::AutoCrouch::enabled)
		return;

	if (!player)
		return;

	cmd->buttons |= IN_BULLRUSH | IN_DUCK;
}

static void RagebotAutoSlow(C_BasePlayer* player, float& forward, float& sideMove, float& bestDamage, C_BaseCombatWeapon* active_weapon, CUserCmd* cmd)
{

	if (!Settings::Ragebot::AutoSlow::enabled){
		return;
	}

	if (!player){
		return;
	}

	float nextPrimaryAttack = active_weapon->GetNextPrimaryAttack();

	if (nextPrimaryAttack > globalVars->curtime)
    {
		return;
	}

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetAmmo() == 0)
		return;

	if( Settings::Ragebot::HitChance::enabled)
	{
		float hc = Ragebothitchance();
		if( hc > Settings::Ragebot::HitChance::value )
		{
			cmd->buttons |= IN_WALK;
			forward = -forward;
			sideMove = -sideMove;
			cmd->upmove = 0;
			return;
		}
		else if( hc == Settings::Ragebot::HitChance::value ) {
			cmd->buttons |= IN_WALK;
			forward = 0;
			sideMove = 0;
			cmd->upmove = 0;
			return;
		}
        else if (cmd->buttons & IN_ATTACK2)
        {
            return;
        }
        else if (cmd->buttons & IN_ATTACK) 
        {
            forward = 0;
			sideMove = 0;
			return;
        }
		else 
		{
			cmd->buttons |= IN_RUN;
			forward = 0;
			sideMove = 0;
			return;
		}
	}

	else if( (active_weapon->GetSpread() + active_weapon->GetInaccuracy()) > (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f)) // https://youtu.be/ZgjYxBRuagA
	{
		cmd->buttons |= IN_WALK;
		forward = -forward;
		sideMove = -sideMove;
		cmd->upmove = 0;
	}

    else if ( (active_weapon->GetSpread() + active_weapon->GetInaccuracy()) == (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f) ) 
    {
        cmd->buttons |= IN_WALK;
		forward = 0;
		sideMove = 0;
		cmd->upmove = 0;
        return;
    }

    else if (cmd->buttons & IN_ATTACK2)
    {
        return;
    }

	else 
	{
		cmd->buttons |= IN_RUN;
		return;
	}
}

static void RagebotAutoCock(C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
    if (!Settings::Ragebot::AutoPistol::enabled)
        return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
        return;

    if(activeWeapon->GetAmmo() == 0)
        return;
    if (cmd->buttons & IN_USE)
        return;

    cmd->buttons |= IN_ATTACK;
    float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
    if (postponeFireReadyTime > 0)
    {
        if (postponeFireReadyTime < globalVars->curtime)
        {
            if (player)
                return;
            cmd->buttons &= ~IN_ATTACK;
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

static void RagebotAutoShoot(C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	//cvar->ConsoleDPrintf("I ma in auto shoot method \n");
	if (!Settings::Ragebot::AutoShoot::enabled)
	{ 
		//cvar->ConsoleDPrintf("Auto Shoot Not Enabled \n");
		return;
	}

	if (!player || activeWeapon->GetAmmo() == 0)
	{
		return;
	}
		

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

	if (cmd->buttons & IN_USE)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (Settings::Ragebot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped())
    {
	    cmd->buttons |= IN_ATTACK2;
	    return; // continue next tick
    }
	
	if( Settings::Ragebot::HitChance::enabled && (Ragebothitchance() < Settings::Ragebot::HitChance::value * 1.5))
	{
		return;
	}	

	float nextPrimaryAttack = activeWeapon->GetNextPrimaryAttack();

    if (!(*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
    {
        if (nextPrimaryAttack > globalVars->curtime)
        {    
            cmd->buttons &= ~IN_ATTACK;
            return;
        }
        else
        {
            cmd->buttons |= IN_ATTACK;
            return;
        }
    }
}

static void FixMouseDeltas(CUserCmd* cmd, const QAngle &angle, const QAngle &oldAngle)
{
    if( !RagebotShouldAim)
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

void Ragebot::CreateMove(CUserCmd* cmd)
{
	if (!Settings::Ragebot::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	
	if (!localplayer || !localplayer->GetAlive())
		return;

	Ragebot::UpdateValues();

	QAngle oldAngle;
	engine->GetViewAngles(oldAngle);
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;

	QAngle angle = cmd->viewangles;
	static QAngle lastRandom = {0,0,0};
	Vector localEye = localplayer->GetEyePosition();

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetInReload())
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

    Vector bestSpot = {0,0,0};
	float bestDamage = 0.0f;
	C_BasePlayer* player = GetClosestPlayerAndSpot(cmd, &bestSpot, &bestDamage);
	if (player)
	{
            //Auto Scop Controll system to controll auto scoping every time
            if (Settings::Ragebot::ScopeControl::enabled)
	        {
                //cheking if the weapon scopable and not scop then it will scop and go back to the next tick
		        if (Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped())
                {
                    return; // will go to the next tick
                }
	        }
            if (cmd->buttons & IN_ATTACK || Settings::Ragebot::AutoShoot::enabled){
                RagebotShouldAim = true;
            }
            else {
                RagebotShouldAim = false;
            }

			Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.
			if (RagebotShouldAim)
			{
				if (Settings::Ragebot::Prediction::enabled)
				{
					localEye = VelocityExtrapolate(localplayer, localEye); // get eye pos next tick
					bestSpot = VelocityExtrapolate(player, bestSpot); // get target pos next tick
				}
				angle = Math::CalcAngle(localEye, bestSpot);

			}
	}
	else // No player to Shoot
	{
        Settings::Debug::AutoAim::target = {0,0,0};
        lastRandom = {0,0,0};
    }

	RagebotAutoCrouch(player, cmd);
	RagebotAutoSlow(player, oldForward, oldSideMove, bestDamage, activeWeapon, cmd);
	RagebotAutoPistol(activeWeapon, cmd);
	RagebotAutoShoot(player, activeWeapon, cmd);
	RagebotAutoCock(player, activeWeapon, cmd);
	RagebotRCS(angle, player, cmd, localplayer);

    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

	FixMouseDeltas(cmd, angle, oldAngle);
	cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);

	if( !Settings::Ragebot::silent )
    	engine->SetViewAngles(cmd->viewangles);

}

void Ragebot::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0 )
	{
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
			return;
		Ragebot::friends.clear();
	}
	if( strcmp(event->GetName(), XORSTR("player_death")) == 0 )
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

void Ragebot::UpdateValues()
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon)
		return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Ragebot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Ragebot::weapons.end())
		index = *activeWeapon->GetItemDefinitionIndex();

	const RagebotWeapon_t& currentWeaponSetting = Settings::Ragebot::weapons.at(index);

	Settings::Ragebot::silent = currentWeaponSetting.silent;
	Settings::Ragebot::friendly = currentWeaponSetting.friendly;
	Settings::Ragebot::AutoAim::fov = currentWeaponSetting.RagebotautoAimFov;
	Settings::Ragebot::AutoPistol::enabled = currentWeaponSetting.autoPistolEnabled;
	Settings::Ragebot::AutoShoot::enabled = currentWeaponSetting.autoShootEnabled;
	Settings::Ragebot::AutoShoot::autoscope = currentWeaponSetting.autoScopeEnabled;
	Settings::Ragebot::HitChance::enabled = currentWeaponSetting.HitChanceEnabled;
	Settings::Ragebot::HitChance::value = currentWeaponSetting.HitChance;
	Settings::Ragebot::AutoWall::value = currentWeaponSetting.autoWallValue;
	Settings::Ragebot::AutoSlow::enabled = currentWeaponSetting.autoSlow;
	Settings::Ragebot::ScopeControl::enabled = currentWeaponSetting.scopeControlEnabled;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		Settings::Ragebot::AutoAim::desiredBones[bone] = currentWeaponSetting.desiredBones[bone];
}
