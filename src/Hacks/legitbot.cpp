 
#include "legitbot.h"
#include "autowall.h"

#include "../Utils/xorstring.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/PerlinNoise.h"


bool Legitbot::aimStepInProgress = false;
std::vector<int64_t> Legitbot::friends = { };
std::vector<long> killTimes = { 0 }; // the Epoch time from when we kill someone

bool shouldAim;
QAngle AimStepLastAngle;
QAngle LegitRCSLPUNCH;

siv::PerlinNoise perlin(1337);

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

bool reactionTime(){
        long currentTime_ms = Util::GetEpochTime();
        static long timeStamp = currentTime_ms;
        long oldTimeStamp;
}
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

static bool IsFlagSetForHitbox(int flags, int hitbox)
{
    switch(hitbox)
    {
        case Hitbox::HITBOX_HEAD:
            return flags & HitboxFlags::HEAD;
        case Hitbox::HITBOX_NECK:
            return flags & HitboxFlags::NECK;
        case Hitbox::HITBOX_PELVIS:
            return flags & HitboxFlags::PELVIS;
        case Hitbox::HITBOX_STOMACH:
            return flags & HitboxFlags::STOMACH;
        case Hitbox::HITBOX_CHEST:
        case Hitbox::HITBOX_LOWER_CHEST:
        case Hitbox::HITBOX_UPPER_CHEST:
            return flags & HitboxFlags::CHEST;
        case Hitbox::HITBOX_LEFT_THIGH:
        case Hitbox::HITBOX_RIGHT_THIGH:
		case Hitbox::HITBOX_LEFT_CALF:
        case Hitbox::HITBOX_RIGHT_CALF:
            return flags & HitboxFlags::LEGS;
        case Hitbox::HITBOX_LEFT_FOOT:
        case Hitbox::HITBOX_RIGHT_FOOT:
            return flags & HitboxFlags::FEET;
        case Hitbox::HITBOX_LEFT_HAND:
        case Hitbox::HITBOX_RIGHT_HAND:
            return flags & HitboxFlags::HANDS;
        case Hitbox::HITBOX_LEFT_UPPER_ARM:
        case Hitbox::HITBOX_RIGHT_UPPER_ARM:
        case Hitbox::HITBOX_LEFT_FOREARM:
        case Hitbox::HITBOX_RIGHT_FOREARM:
            return flags & HitboxFlags::ARMS;
        default: // invalid hitbox?
		{
			cvar->ConsoleDPrintf(XORSTR("Warning: Wrong hitbox (%d) fed into IsFlagSetForHitbox\n"), hitbox);
            return false;
		}
	}
}
static float AutoWallBestSpot(C_BasePlayer *player, Vector &bestSpot)
{
	model_t *model = player->GetModel();
	if (!model)
		return 0.f;
	
	studiohdr_t *hdr = modelInfo->GetStudioModel(model);
	if (!hdr)
		return 0.f;

	mstudiohitboxset_t *hitboxSet = hdr->pHitboxSet(player->GetHitboxSetCount());
	if (!hitboxSet)
		return 0.f;

	float bestDamage = Settings::Legitbot::AutoWall::value;
	const int hitboxFlags = Settings::Legitbot::AutoAim::desiredHitboxes;


	for ( int i = 0; i < hitboxSet->numhitboxes; i++ )	
	{
		if( !IsFlagSetForHitbox(hitboxFlags, i) )
		continue;
	
		mstudiobbox_t* hitbox = hitboxSet->pHitbox(i);

		if ( !hitbox )
			continue;

		if( i == Hitbox::HITBOX_HEAD ) // head multipoint
		{
			Vector headPoints[headVectors];
			if( !HeadMultiPoint(player, headPoints) )
				continue;
			for( int j = 0; j < headVectors; j++ )
			{
				AutoWall::FireBulletData data;
				float spotDamage = AutoWall::GetDamage(headPoints[j], !Settings::Legitbot::friendly, data);
				if( spotDamage > bestDamage )
				{
					bestSpot = headPoints[j];
					if( spotDamage > player->GetHealth() )
						return spotDamage;
					bestDamage = spotDamage;
				}
			}
		}
		int boneID = hitbox->bone;

		Vector bone3D = player->GetBonePosition(boneID);
	
		AutoWall::FireBulletData data;
		float boneDamage = AutoWall::GetDamage(bone3D, !Settings::Legitbot::friendly, data);
		if( boneDamage > bestDamage )
		{
			bestSpot = bone3D;
			if( boneDamage > player->GetHealth() )
				return boneDamage;

			bestDamage = boneDamage;
		}
	}
	return bestDamage;
}
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

static Vector VelocityExtrapolate(C_BasePlayer* player, Vector aimPos)
{
	return aimPos + (player->GetVelocity() * globalVars->interval_per_tick);
}

/* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) */
static Vector GetClosestSpot( CUserCmd* cmd, C_BasePlayer* localPlayer, C_BasePlayer* enemy, AimTargetType aimTargetType = AimTargetType::FOV)
{
	model_t *model = enemy->GetModel();
	if ( !model )
		return Vector(0.f, 0.f, 0.f);

	studiohdr_t *hdr = modelInfo->GetStudioModel( model );
	if ( !hdr )
		return Vector(0.f, 0.f, 0.f);

	mstudiohitboxset_t *hitboxSet = hdr->pHitboxSet( enemy->GetHitboxSetCount() );
	if ( !hitboxSet )
		return Vector(0.f, 0.f, 0.f);
	
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	float tempFov = Settings::Legitbot::AutoAim::fov;
	float tempDistance = Settings::Legitbot::AutoAim::fov * 5.f;

	Vector pVecTarget = localPlayer->GetEyePosition();

	Vector tempSpot = {0,0,0};

	const int hitboxFlags = Settings::Legitbot::AutoAim::desiredHitboxes;
	for ( int i = 0; i < hitboxSet->numhitboxes; i++ )
	{
		if(!IsFlagSetForHitbox(hitboxFlags, i))			
			continue;

		mstudiobbox_t* hitbox = hitboxSet->pHitbox(i);

		if (!hitbox)
			continue;
			
		int boneID = hitbox->bone;		
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
		else if( aimTargetType == AimTargetType::REAL_DISTANCE )
		{
			float cbDistance = pVecTarget.DistTo(cbVecTarget);
			float cbRealDistance = GetRealDistanceFOV(cbDistance, Math::CalcAngle(pVecTarget, cbVecTarget), cmd);

			if( cbRealDistance < tempDistance )
			{
				if( Entity::IsVisibleThroughEnemies(enemy, boneID) )
				{
					tempDistance = cbRealDistance;
					tempSpot = cbVecTarget;
				}
			}
		}
	}
	return tempSpot;
}

static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd, bool visibleCheck, Vector* bestSpot, float* bestDamage, AimTargetType aimTargetType = AimTargetType::FOV)
{
	if (Settings::Legitbot::AutoAim::realDistance)
		aimTargetType = AimTargetType::REAL_DISTANCE;

	static C_BasePlayer* lockedOn = nullptr;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BasePlayer* closestEntity = nullptr;

	float bestFov = Settings::Legitbot::AutoAim::fov;
	float bestRealDistance = Settings::Legitbot::AutoAim::fov * 5.f;

	if( lockedOn )
	{
		if( lockedOn->GetAlive() && !Settings::Legitbot::AutoAim::closestHitbox && !Entity::IsSpotVisibleThroughEnemies(lockedOn, lockedOn->GetBonePosition((int)Settings::Legitbot::bone)) )
		{
			lockedOn = nullptr;
			return nullptr;
		}
		if (!(cmd->buttons & IN_ATTACK || inputSystem->IsButtonDown(Settings::Legitbot::aimkey)) || lockedOn->GetDormant())//|| !Entity::IsVisible(lockedOn, bestBone, 180.f, Settings::ESP::Filters::smokeCheck))
		{
			lockedOn = nullptr;
		}
		else
		{
			if( !lockedOn->GetAlive() )
			{
				if( Settings::Legitbot::AutoAim::engageLockTR )
				{
					if(Util::GetEpochTime() - killTimes.back() > Settings::Legitbot::AutoAim::engageLockTTR) // if we got the kill over the TTR time, engage another foe.
					{
						lockedOn = nullptr;
					}
				}
				return nullptr;
			}

			if( Settings::Legitbot::AutoAim::closestHitbox )
			{
				Vector tempSpot = GetClosestSpot(cmd, localplayer, lockedOn, aimTargetType);
				if( tempSpot.IsZero() )
				{
					return nullptr;
				}
				*bestSpot = tempSpot;
			}
			else
			{
				*bestSpot = lockedOn->GetBonePosition((int)Settings::Legitbot::bone);
			}

			return lockedOn;
		}
	}

 	for (int i = engine->GetMaxClients(); i > 0 ; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player
			|| player == localplayer
			|| player->GetDormant()
			|| !player->GetAlive()
			|| player->GetImmune())
			continue;

		if (!Settings::Legitbot::friendly && Entity::IsTeamMate(player, localplayer))
			continue;

		if( !Legitbot::friends.empty() ) // check for friends, if any
		{
			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			if (std::find(Legitbot::friends.begin(), Legitbot::friends.end(), entityInformation.xuid) != Legitbot::friends.end())
				continue;
		}

		Legitbot::targetAimbot = i;
		Vector eVecTarget = player->GetBonePosition((int) Settings::Legitbot::bone);
		if( Settings::Legitbot::AutoAim::closestHitbox )		
		{
			Vector tempSpot = GetClosestSpot(cmd, localplayer, player, aimTargetType);
			if( tempSpot.IsZero() || !Entity::IsSpotVisibleThroughEnemies(player, tempSpot) )
				continue;
			eVecTarget = tempSpot;
		}

		Vector pVecTarget = localplayer->GetEyePosition();
        lastRayStart = pVecTarget;
        lastRayEnd = eVecTarget;

		QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

		float distance = pVecTarget.DistTo(eVecTarget);
		float fov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, eVecTarget));

		if (aimTargetType == AimTargetType::FOV && fov > bestFov)
			continue;

		float realDistance = GetRealDistanceFOV(distance, Math::CalcAngle(pVecTarget, eVecTarget), cmd);

		if (aimTargetType == AimTargetType::REAL_DISTANCE && realDistance > bestRealDistance)
			continue;
		if (visibleCheck && !Settings::Legitbot::AutoWall::enabled && !Entity::IsSpotVisible(player, eVecTarget))
			continue;
		if ( Settings::Legitbot::SmokeCheck::enabled && LineGoesThroughSmoke( localplayer->GetEyePosition( ), eVecTarget, true ) )
			continue;
		if ( Settings::Legitbot::FlashCheck::enabled && localplayer->IsFlashed() )
			continue;

		if (Settings::Legitbot::AutoWall::enabled)
		{
			Vector wallBangSpot = {0,0,0};
			float damage = AutoWallBestSpot(player, wallBangSpot); // sets Vector Angle, returns damage of hitting that spot.

			if( !wallBangSpot.IsZero() )
			{
				*bestDamage = damage;
				*bestSpot = wallBangSpot;
				closestEntity = player;
				lastRayEnd = wallBangSpot;
			}
		}
		else
		{
			closestEntity = player;
			*bestSpot = eVecTarget;
			bestFov = fov;
			bestRealDistance = realDistance;
		}
	}
	if( Settings::Legitbot::AutoAim::engageLock )
	{
		if( !lockedOn )
		{
			if( (cmd->buttons & IN_ATTACK) || inputSystem->IsButtonDown(Settings::Legitbot::aimkey) )
			{
				if( Util::GetEpochTime() - killTimes.back() > 100 ) // if we haven't gotten a kill in under 100ms.
				{
					lockedOn = closestEntity; // This is to prevent a Rare condition when you one-tap someone without the aimbot, it will lock on to another target.
				}
			}
			else
			{
				return nullptr;
			}
		}
	}
	if( bestSpot->IsZero() )
		return nullptr;

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

static void RCS(QAngle& angle, C_BasePlayer* player, CUserCmd* cmd)
{
	if (!Settings::Legitbot::RCS::enabled)
		return;

	if (!(cmd->buttons & IN_ATTACK))
		return;

	bool hasTarget = Settings::Legitbot::AutoAim::enabled && shouldAim && player;

	if (!Settings::Legitbot::RCS::always_on && !hasTarget)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	if ( Settings::Legitbot::silent || hasTarget )
	{
		angle.x -= CurrentPunch.x * Settings::Legitbot::RCS::valueX;
		angle.y -= CurrentPunch.y * Settings::Legitbot::RCS::valueY;
	}
	else if (localplayer->GetShotsFired() > 1)
	{
		QAngle NewPunch = { CurrentPunch.x - LegitRCSLPUNCH.x, CurrentPunch.y - LegitRCSLPUNCH.y, 0 };

		angle.x -= NewPunch.x * Settings::Legitbot::RCS::valueX;
		angle.y -= NewPunch.y * Settings::Legitbot::RCS::valueY;
	}

	LegitRCSLPUNCH = CurrentPunch;
}
static void AimStep(C_BasePlayer* player, QAngle& angle, CUserCmd* cmd)
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

static void Smooth(C_BasePlayer* player, QAngle& angle)
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

static void AutoCrouch(C_BasePlayer* player, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoCrouch::enabled)
		return;

	if (!player)
		return;

	cmd->buttons |= IN_BULLRUSH | IN_DUCK;
}

static void AutoSlow(C_BasePlayer* player, float& forward, float& sideMove, float& bestDamage, C_BaseCombatWeapon* active_weapon, CUserCmd* cmd)
{

	if (!Settings::Legitbot::AutoSlow::enabled){
		return;
	}

	if (!player){
		return;
	}

	float nextPrimaryAttack = active_weapon->GetNextPrimaryAttack();

	if (nextPrimaryAttack > globalVars->curtime){
		return;
	}

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetAmmo() == 0)
		return;

	 if( localplayer->GetVelocity().Length() > (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3) ) // https://youtu.be/ZgjYxBRuagA
	{
		cmd->buttons |= IN_WALK;
		forward = -forward;
		sideMove = -sideMove;
		cmd->upmove = 0;
	}
}

static void AutoCock(C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
    if (!Settings::Legitbot::AutoShoot::enabled)
        return;

    if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
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

static void AutoShoot(C_BasePlayer* player, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoShoot::enabled)
		return;

	if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
		return;

	if (!player || activeWeapon->GetAmmo() == 0)
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

	if (cmd->buttons & IN_USE)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (Settings::Legitbot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped())
    {
	    cmd->buttons |= IN_ATTACK2;
	    return; // continue next tick
    }

	if( Settings::Legitbot::AutoShoot::velocityCheck && localplayer->GetVelocity().Length() > (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3) )
		return;

	float nextPrimaryAttack = activeWeapon->GetNextPrimaryAttack();

    if (!(*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
    {
        if (nextPrimaryAttack > globalVars->curtime)
            cmd->buttons &= ~IN_ATTACK;
        else
            cmd->buttons |= IN_ATTACK;
    }
}

static void NoShoot(C_BaseCombatWeapon* activeWeapon, C_BasePlayer* player, CUserCmd* cmd)
{
	if (player && Settings::Legitbot::NoShoot::enabled)
	{
		if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_C4)
			return;

		if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
			cmd->buttons &= ~IN_ATTACK2;
		else
			cmd->buttons &= ~IN_ATTACK;
	}
}

static void FixMouseDeltas(CUserCmd* cmd, const QAngle &angle, const QAngle &oldAngle)
{
    if( !shouldAim )
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
void Legitbot::CreateMove(CUserCmd* cmd)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	Legitbot::UpdateValues();

	if (!Settings::Legitbot::enabled)
		return;

	QAngle oldAngle;
	engine->GetViewAngles(oldAngle);
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;

	QAngle angle = cmd->viewangles;
	static bool newTarget = true;
	static QAngle lastRandom = {0,0,0};
	Vector localEye = localplayer->GetEyePosition();

	shouldAim = Settings::Legitbot::AutoShoot::enabled;

	if (Settings::Legitbot::IgnoreJump::enabled && (!(localplayer->GetFlags() & FL_ONGROUND) && localplayer->GetMoveType() != MOVETYPE_LADDER))
		return;

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetInReload())
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

	if (Settings::Legitbot::ScopeControl::enabled)
	{
		if (Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped())
			return;
	}

    Vector bestSpot = {0,0,0};
	float bestDamage = 0.0f;
	C_BasePlayer* player = GetClosestPlayerAndSpot(cmd, !Settings::Legitbot::AutoWall::enabled, &bestSpot, &bestDamage);

	if (player)
	{
		if (Settings::Legitbot::IgnoreEnemyJump::enabled && (!(player->GetFlags() & FL_ONGROUND) && player->GetMoveType() != MOVETYPE_LADDER))
			return;

		if (Settings::Legitbot::AutoAim::enabled)
		{
			if (cmd->buttons & IN_ATTACK && !Settings::Legitbot::aimkeyOnly)
				shouldAim = true;

			if (inputSystem->IsButtonDown(Settings::Legitbot::aimkey))
				shouldAim = true;

			if(!Settings::Legitbot::CourseRandomization::enabled){

			    Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.
			}

			//Only start aimbotting after we reach X shot number in the spray. - Crazily
			if (Settings::Legitbot::DoAimAfterXShots::enabled)
			{
			    int shotsFired = localplayer->GetShotsFired();
			    //I think c++ always rounds down but only way to do it with IMGUI Slider anyway.
			    if(shotsFired <= (int) Settings::Legitbot::DoAimAfterXShots::value)
			    {

			        shouldAim = false;
			    } else if (shotsFired >= Settings::Legitbot::DoAimAfterXShots::value)
			        {
			            shouldAim = true;
			        }

			}

			if (shouldAim)
			{

				if (Settings::Legitbot::Prediction::enabled)
				{
					localEye = VelocityExtrapolate(localplayer, localEye); // get eye pos next tick
					bestSpot = VelocityExtrapolate(player, bestSpot); // get target pos next tick
				}
				angle = Math::CalcAngle(localEye, bestSpot);

				if (Settings::Legitbot::ErrorMargin::enabled)
				{
					static int lastShotFired = 0;
					if ((localplayer->GetShotsFired() > lastShotFired) || newTarget) //get new random spot when firing a shot or when aiming at a new target
						lastRandom = ApplyErrorToAngle(&angle, Settings::Legitbot::ErrorMargin::value);

					angle += lastRandom;
					lastShotFired = localplayer->GetShotsFired();
				}
				//This is kindof dirty but provides the desired effect.
				// Maybe someone better at c++ than I am can edit it later - Crazily & Conarium Software

			    	if (Settings::Legitbot::CourseRandomization::enabled)
				{
				    Vector bestSpotOld = bestSpot;

				    const double wavelength = 3;
				    const double amplitude = Settings::Legitbot::CourseRandomization::value;

				    //GetSimulationTime was used because we just needed a rapidly changing number.
				    double xVar = localplayer->GetSimulationTime();
				    double yVar = localplayer->GetSimulationTime() - 5;
				    double zVar = localplayer->GetSimulationTime() + 5;

				    double xError = perlin.normalizedOctaveNoise2D(xVar/wavelength,yVar/wavelength, 2) * amplitude;
				    double yError = perlin.normalizedOctaveNoise2D(yVar/wavelength,zVar/wavelength, 2) * amplitude;

				    bestSpot.x += xError;
				    bestSpot.z += yError;

				    Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.

				}
			    	angle = Math::CalcAngle(localEye, bestSpot);
				newTarget = false;
			}
		}
	}
	else // No player to Shoot
	{
        Settings::Debug::AutoAim::target = {0,0,0};
        newTarget = true;
        lastRandom = {0,0,0};
    }

     AimStep(player, angle, cmd);
	AutoCrouch(player, cmd);
	AutoSlow(player, oldForward, oldSideMove, bestDamage, activeWeapon, cmd);
	AutoPistol(activeWeapon, cmd);
	AutoShoot(player, activeWeapon, cmd);
	AutoCock(player, activeWeapon, cmd);
	RCS(angle, player, cmd);
	Smooth(player, angle);
	NoShoot(activeWeapon, player, cmd);

	Math::NormalizeAngles(angle);
    	Math::ClampAngles(angle);

	FixMouseDeltas(cmd, angle, oldAngle);
	cmd->viewangles = angle;

    	Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);

	if( !Settings::Legitbot::silent )
    	engine->SetViewAngles(cmd->viewangles);
}
void Legitbot::FireGameEvent(IGameEvent* event)
{
	if (!event)
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
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon)
		return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
		index = *activeWeapon->GetItemDefinitionIndex();

	const LegitWeapon_t& currentWeaponSetting = Settings::Legitbot::weapons.at(index);

	Settings::Legitbot::silent = currentWeaponSetting.silent;
	Settings::Legitbot::friendly = currentWeaponSetting.friendly;
	Settings::Legitbot::bone = currentWeaponSetting.bone;
	Settings::Legitbot::aimkey = currentWeaponSetting.aimkey;
	Settings::Legitbot::aimkeyOnly = currentWeaponSetting.aimkeyOnly;
	Settings::Legitbot::Smooth::enabled = currentWeaponSetting.smoothEnabled;
	Settings::Legitbot::CourseRandomization::enabled = currentWeaponSetting.courseRandomizationEnabled;
     Settings::Legitbot::DoAimAfterXShots::enabled = currentWeaponSetting.doAimAfterXShotsEnabled;
	Settings::Legitbot::Smooth::value = currentWeaponSetting.smoothAmount;
	Settings::Legitbot::CourseRandomization::value = currentWeaponSetting.courseRandomizationAmount;
     Settings::Legitbot::DoAimAfterXShots::value = currentWeaponSetting.doAimAfterXShotsAmount;
	Settings::Legitbot::Smooth::type = currentWeaponSetting.smoothType;
	Settings::Legitbot::ErrorMargin::enabled = currentWeaponSetting.errorMarginEnabled;
	Settings::Legitbot::ErrorMargin::value = currentWeaponSetting.errorMarginValue;
	Settings::Legitbot::AutoAim::enabled = currentWeaponSetting.autoAimEnabled;
	Settings::Legitbot::AutoAim::fov = currentWeaponSetting.LegitautoAimFov;
	Settings::Legitbot::AutoAim::engageLock = currentWeaponSetting.engageLock;
	Settings::Legitbot::AutoAim::engageLockTR = currentWeaponSetting.engageLockTR;
	Settings::Legitbot::AutoAim::engageLockTTR = currentWeaponSetting.engageLockTTR;
	Settings::Legitbot::AutoAim::closestHitbox = currentWeaponSetting.closestHitbox;		
	Settings::Legitbot::AimStep::enabled = currentWeaponSetting.aimStepEnabled;
	Settings::Legitbot::AimStep::min = currentWeaponSetting.aimStepMin;
	Settings::Legitbot::AimStep::max = currentWeaponSetting.aimStepMax;
	Settings::Legitbot::AutoPistol::enabled = currentWeaponSetting.autoPistolEnabled;
	Settings::Legitbot::AutoShoot::enabled = currentWeaponSetting.autoShootEnabled;
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
	Settings::Legitbot::SpreadLimit::enabled = currentWeaponSetting.spreadLimitEnabled;
	Settings::Legitbot::SpreadLimit::value = currentWeaponSetting.spreadLimit;
	Settings::Legitbot::AutoWall::enabled = currentWeaponSetting.autoWallEnabled;
	Settings::Legitbot::AutoWall::value = currentWeaponSetting.autoWallValue;
	Settings::Legitbot::AutoSlow::enabled = currentWeaponSetting.autoSlow;
	Settings::Legitbot::ScopeControl::enabled = currentWeaponSetting.scopeControlEnabled;
	Settings::Legitbot::ShootAssist::Hitchance::enabled = currentWeaponSetting.hitchanceEnaled;
     Settings::Legitbot::ShootAssist::Hitchance::value = currentWeaponSetting.hitchance;

	Settings::Legitbot::AutoAim::desiredHitboxes = currentWeaponSetting.desiredHitboxes;

	Settings::Legitbot::AutoAim::realDistance = currentWeaponSetting.autoAimRealDistance;
}
