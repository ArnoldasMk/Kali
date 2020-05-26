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

bool shouldAim;
inline int delay = 0, shotFIred = 0;
inline bool _delayed = false; // to determaine all ready delayed or not
QAngle AimStepLastAngle;
QAngle RCSLastPunch = {0,0,0};

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

static bool IsIzFov(C_BasePlayer* player, CUserCmd *cmd, const Vector& pVecTarget, const Vector& cbVecTarget, AimTargetType aimTargetType = AimTargetType::FOV)
{
	using namespace Settings::Legitbot::AutoAim;
	float cbFov = 0.0;
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);
	float realDistance = 0.f;
	if (Settings::Legitbot::AutoAim::realDistance)
	{
		aimTargetType = AimTargetType::REAL_DISTANCE;
		float distance = pVecTarget.DistTo(cbVecTarget);
		realDistance = GetRealDistanceFOV(distance, Math::CalcAngle(pVecTarget, cbVecTarget), cmd);	
	}
		
	cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
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
/* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) */
static C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer, CUserCmd* cmd, AimTargetType aimTargetType = AimTargetType::FOV)
{
	C_BasePlayer* closenstEntity = nullptr;
	float bestFov = Settings::Legitbot::AutoAim::fov;
	float bestRealDistance = Settings::Legitbot::AutoAim::fov * 5.0f;

	float prevDistance = 0.f,
	  		cbFov = 0.f;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	if (Settings::Legitbot::AutoAim::realDistance)
		aimTargetType = AimTargetType::REAL_DISTANCE;

	
	for (int i = 1; i < engine->GetMaxClients(); ++i)
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

		Vector cbVecTarget = player->GetAbsOrigin();
		cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
		float distance = pVecTarget.DistTo(cbVecTarget);
		float realDistance = GetRealDistanceFOV(distance, Math::CalcAngle(pVecTarget, cbVecTarget), cmd);
		
		if (aimTargetType == AimTargetType::FOV && cbFov > bestFov)
			continue;
		if (aimTargetType == AimTargetType::REAL_DISTANCE && realDistance > bestRealDistance)
			continue;

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

/* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) */
static Vector GetClosestSpot( CUserCmd* cmd, C_BasePlayer* localPlayer, C_BasePlayer* enemy, AimTargetType aimTargetType = AimTargetType::FOV)
{
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	float tempFov = Settings::Legitbot::AutoAim::fov;
	float tempDistance = Settings::Legitbot::AutoAim::fov * 5.f;

	Vector pVecTarget = localPlayer->GetEyePosition();

	Vector tempSpot = {0,0,0};

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

		Autowall::FireBulletData data;
		float boneDamage = Autowall::GetDamage(bone3D, !Settings::Legitbot::friendly, data);

		Vector cbVecTarget = enemy->GetBonePosition(boneID);

		if( aimTargetType == AimTargetType::FOV )
		{
			float cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));

			if( cbFov < tempFov )
			{
				if( Entity::IsVisibleThroughEnemies(enemy, boneID) && boneDamage >= Settings::Legitbot::minDamage )
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
				if( Entity::IsVisibleThroughEnemies(enemy, boneID) && boneDamage >= Settings::Legitbot::minDamage)
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
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BasePlayer* closestEntity = nullptr;

	C_BasePlayer *player = GetClosestEnemy(localplayer, cmd); // getting the closest enemy to the crosshair

	if (player == nullptr)
		return nullptr;

	Vector eVecTarget = {0,0,0};
	auto IsPriorityBoneINFov = IsIzFov( player, cmd, localplayer->GetEyePosition(), player->GetAbsOrigin() );
	if ( !IsPriorityBoneINFov )
	{
		Vector tempSpot = GetClosestSpot(cmd, localplayer, player, aimTargetType);
		if( tempSpot.IsZero() || !Entity::IsSpotVisibleThroughEnemies(player, tempSpot) )
			return nullptr;
		eVecTarget = tempSpot;
	}
	else if (IsPriorityBoneINFov)
	{
		eVecTarget = player->GetBonePosition((int) Settings::Legitbot::bone);
	}
	
	if (visibleCheck && !Entity::IsSpotVisible(player, eVecTarget))
		return nullptr;
	if ( Settings::Legitbot::SmokeCheck::enabled && LineGoesThroughSmoke( localplayer->GetEyePosition( ), eVecTarget, true ) )
		return nullptr;
	if ( Settings::Legitbot::FlashCheck::enabled && localplayer->IsFlashed() )
		return nullptr;


	closestEntity = player;
	*bestSpot = eVecTarget;

	if( bestSpot->IsZero() )
		return nullptr;

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
		QAngle NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * Settings::Legitbot::RCS::valueX;
		angle.y -= NewPunch.y * Settings::Legitbot::RCS::valueY;
	}

	RCSLastPunch = CurrentPunch;
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

float hitchance()
{
	float hitchance = 101;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (activeWeapon)
	{
		if (Settings::Legitbot::Hitchance::value > 0)
		{
			float inaccuracy = activeWeapon->GetInaccuracy();
			if (inaccuracy == 0) inaccuracy = 0.0000001;
			inaccuracy = 1 / inaccuracy;
			hitchance = inaccuracy;
		}
		return hitchance;
	}
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

	if (nextPrimaryAttack > globalVars->curtime)
    {
		return;
	}

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetAmmo() == 0)
		return;

	if( Settings::Legitbot::Hitchance::enabled)
	{
		float hc = hitchance();
		if( hc > Settings::Legitbot::Hitchance::value )
		{
			cmd->buttons |= IN_WALK;
			forward = -forward;
			sideMove = -sideMove;
			cmd->upmove = 0;
			return;
		}
		else if( hc == Settings::Legitbot::Hitchance::value ) {
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
    {
        return true;
    }
    else if (inputSystem->IsButtonDown(Settings::Legitbot::aimkey))
	{
        return true;
    }
    else
    {
        return false;
    }
}

// void ShootAssist(CUserCmd* cmd, C_BasePlayer* localplayer, C_BasePlayer* player) 
// {
// 	if (!Settings::Legitbot::ShootAssist::enabled)
// 		return;
// 	/*cheking the player is near my cross hair or not
// 	// if it is then it will reduce the mouse sencitivity
// 	*/
// 	if ( GetClosestSpot(cmd, localplayer, player).IsZero())
// 		return; 
	
// 	cmd->buttons |= IN_ATTACK;
// 	shouldAim = true;
// }
void Legitbot::CreateMove(CUserCmd* cmd)
{

	if(!Settings::Legitbot::enabled)
		return;
			
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	
	if (!localplayer || !localplayer->GetAlive())
		return;

	Legitbot::UpdateValues();

	QAngle oldAngle;
	engine->GetViewAngles(oldAngle);
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;

	QAngle angle = cmd->viewangles;
	static bool newTarget = true;
	static QAngle lastRandom = {0,0,0};
	Vector localEye = localplayer->GetEyePosition();

	if (Settings::Legitbot::IgnoreJump::enabled && (!(localplayer->GetFlags() & FL_ONGROUND) && localplayer->GetMoveType() != MOVETYPE_LADDER))
		return;

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetInReload())
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

    Vector bestSpot = {0,0,0};
	float bestDamage = 0.0f;
	C_BasePlayer* player = GetClosestPlayerAndSpot(cmd, !Settings::Legitbot::AutoWall::enabled, &bestSpot, &bestDamage);

	if (player)
	{
		//Auto Matic place the crosshair in to the player
        if (Settings::Legitbot::AutoAim::enabled)
		{
            //Do not shoot if player is in air or in ladder 
            if (Settings::Legitbot::IgnoreEnemyJump::enabled && (!(player->GetFlags() & FL_ONGROUND) && player->GetMoveType() != MOVETYPE_LADDER))
		    	{return;}

            //Auto Scop Controll system to controll auto scoping every time
            if (Settings::Legitbot::ScopeControl::enabled)
	        {
                //cheking if the weapon scopable and not scop then it will scop and go back to the next tick
		        if (Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped())
                {
                    cmd->buttons |= IN_ATTACK2;
                    return; // will go to the next tick
                }
			
	        }
            // Conditions if AimKeyOnly enabled
            if (Settings::Legitbot::aimkeyOnly)
            {
				shouldAim = AimKeyOnly(cmd);
                
            }
			else if( cmd->buttons & IN_ATTACK)
			{
				shouldAim = true;
			}
			else if (Settings::Legitbot::ShootAssist::enabled) {
				if (!(GetClosestSpot(cmd, localplayer, player).IsZero()) || !newTarget)
				{
					if(Settings::Legitbot::Hitchance::enabled)
					{
						if(hitchance() > Settings::Legitbot::Hitchance::value * 1.5)
						{
							if(delay >= (Settings::Legitbot::ShotDelay::value / 10.f) && _delayed == false)
							{
								_delayed = true;
								shouldAim = false;
							}
							if (_delayed)
							{
								cmd->buttons |= IN_ATTACK;
								shouldAim = true;
							}
							else
							{
								shouldAim = false;
							}
							
							delay++;
						}
					}	
					else 
					{
						if(delay >= (Settings::Legitbot::ShotDelay::value / 10.f) && _delayed == false)
							{
								_delayed = true;
								shouldAim = false;
							}
							if (_delayed)
							{
								cmd->buttons |= IN_ATTACK;
								shouldAim = true;
							}
							else 
							{
								shouldAim = false;
							}
							delay++;
					}
				}				
			}
            else {
                shouldAim = false;
            }

			Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.

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
				newTarget = false;
			}
		}
		else 
		{
			Settings::Legitbot::ShootAssist::enabled = false;
		}
	}
	else // No player to Shoot
	{
        Settings::Debug::AutoAim::target = {0,0,0};
        newTarget = true;
        lastRandom = {0,0,0};
		delay = 0;
		// _delayed = false;
		shotFIred = localplayer->GetShotsFired();
		if(_delayed)
		{
			if(shotFIred <= Settings::Legitbot::MinShotFire::value && shotFIred > 0)
			{
				cmd->buttons |= IN_ATTACK;
			}
			else
			{
				shotFIred = 0;
				_delayed = false;
			}	
		}
			
    }

	AimStep(player, angle, cmd);
	AutoCrouch(player, cmd);
	AutoSlow(player, oldForward, oldSideMove, bestDamage, activeWeapon, cmd);
	AutoPistol(activeWeapon, cmd);
	// ShootAssist(cmd, localplayer, player);
	//AutoShoot(player, activeWeapon, cmd);
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
	Settings::Legitbot::Hitchance::enabled = currentWeaponSetting.hitchanceEnaled;
	Settings::Legitbot::Hitchance::value = currentWeaponSetting.hitchance;
	Settings::Legitbot::ShotDelay::value = currentWeaponSetting.shotDelay;
	Settings::Legitbot::MinShotFire::value = currentWeaponSetting.minShotFire;
	Settings::Legitbot::AutoWall::enabled = currentWeaponSetting.autoWallEnabled;
	Settings::Legitbot::minDamage = currentWeaponSetting.MinDamage;
	Settings::Legitbot::AutoSlow::enabled = currentWeaponSetting.autoSlow;
	Settings::Legitbot::ScopeControl::enabled = currentWeaponSetting.scopeControlEnabled;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		Settings::Legitbot::AutoAim::desiredBones[bone] = currentWeaponSetting.desiredBones[bone];

	Settings::Legitbot::AutoAim::realDistance = currentWeaponSetting.autoAimRealDistance;
}
