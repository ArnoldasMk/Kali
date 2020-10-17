// #pragma GCC diagnostic ignored "-Wcomment"
// #pragma GCC diagnostic ignored "-Warray-bounds"

#include "ragebot.h"
#include "resolver.h"
#include "../Utils/draw.h"
#define absolute(x) ( x = x < 0 ? x * -1 : x)
#define RandomeFloat(x) (static_cast<double>( static_cast<double>(std::rand())/ static_cast<double>(RAND_MAX/x)))
#define NormalizeNo(x) (x = (x < 0) ? ( x * -1) : x )
Vector quickpeekstartpos = Vector{ 0, 0, 0 };
#define TIME_TO_TICKS( dt )	( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICK_INTERVAL globalVars->interval_per_tick
std::vector<int64_t> Ragebot::friends = {};
std::vector<long> RagebotkillTimes = { 0 }; // the Epoch time from when we kill someone
Vector *bulletPosition = new Vector();
QAngle RCSLastPunch;
bool hasShot;
void GetDamageAndSpots(C_BasePlayer* player, Vector &Spot, int& Damage, int& playerHelth, int& i,const std::unordered_map<int, int>* modelType, const RageWeapon_t& currentSetting)
{
	if (!player || !player->GetAlive() || !currentSetting.desireBones[i])
		return;

	static auto HitboxHead([&](int BoneID){
		Spot = player->GetBonePosition(BoneID);
		Ragebot::ragebotPredictionSystem->BestHeadPoint(player, BoneID, Damage, Spot);
		// BestMultiPointHEADDamage(player, BoneID, Damage, Spot);
	});
	static auto UpperSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Ragebot::ragebotPredictionSystem->BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 80 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {	BONE_LEFT_COLLARBONE, 
									BONE_RIGHT_COLLARBONE,
									BONE_LEFT_SHOULDER, 
									BONE_RIGHT_SHOULDER
								};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
				return;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 70)
				return;
		}
	});
	static auto MiddleSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Ragebot::ragebotPredictionSystem->BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 80 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {
									BONE_LEFT_ARMPIT,
    								BONE_RIGHT_ARMPIT,
									BONE_LEFT_BICEP,
    								BONE_RIGHT_BICEP,
									BONE_LEFT_ELBOW,
    								BONE_RIGHT_ELBOW
								};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
				return;
			}
			else if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			else if (bestDamage >= 70)
				return;
		}
	});
	static auto LowerSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Ragebot::ragebotPredictionSystem->BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 80 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {	
									BONE_LEFT_FOREARM,
    								BONE_LEFT_WRIST,
    								BONE_RIGHT_FOREARM,
    								BONE_RIGHT_WRIST,
								};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
				return;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 70)
				return;
		}
	});
	static auto HipHitbox([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Ragebot::ragebotPredictionSystem->BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 40 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {BONE_LEFT_BUTTCHEEK,
    								BONE_LEFT_THIGH,
    								BONE_RIGHT_BUTTCHEEK,
    								BONE_RIGHT_THIGH,
								};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
				return;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 70)
				return;
		}
	});
	static auto PelvisHitbox([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Ragebot::ragebotPredictionSystem->BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 40 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {BONE_LEFT_KNEE, 
									BONE_LEFT_ANKLE,
									BONE_LEFT_SOLE, 
									BONE_RIGHT_BUTTCHEEK,
									BONE_RIGHT_THIGH,
									BONE_RIGHT_KNEE,
									BONE_RIGHT_ANKLE,
									BONE_RIGHT_SOLE};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
				return;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 70)
				return;
		}
	});
	static auto DefaultHitbox([&](int BoneID){
		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
	});

	int boneID = -1;

	switch (static_cast<DesireBones>(i))
	{
		case DesireBones::BONE_HEAD:
			boneID = (*modelType).at(BONE_HEAD);
			if ( playerHelth <= 85  ) 
				boneID = (*modelType).at(BONE_NECK);
			if (currentSetting.desiredMultiBones[i]) 
				HitboxHead(boneID); // lamda expression because again creating a new method is going to make the source code mess :p
			else 
				DefaultHitbox(boneID);
			break;
		
		case DesireBones::UPPER_CHEST:
			boneID = (*modelType).at(BONE_UPPER_SPINAL_COLUMN);
			if (currentSetting.desiredMultiBones[i]) 
				UpperSpine(boneID);
			else 
				DefaultHitbox(boneID);
			break;
			
		case DesireBones::MIDDLE_CHEST:
			boneID = (*modelType).at(BONE_MIDDLE_SPINAL_COLUMN);
			if (currentSetting.desiredMultiBones[i]) 
				MiddleSpine(boneID);
			else 
				DefaultHitbox(boneID);
			break;
		
		case DesireBones::LOWER_CHEST:
			boneID = (*modelType).at(BONE_LOWER_SPINAL_COLUMN);
			if (currentSetting.desiredMultiBones[i]) 
				LowerSpine(boneID);
			else 
				DefaultHitbox(boneID);
			break;
		
		case DesireBones::BONE_HIP:
			boneID = (*modelType).at(BONE_HIP);
			if (currentSetting.desiredMultiBones[i]) 
				HipHitbox(boneID);
			else 
				DefaultHitbox(boneID);
			break;
		
		case DesireBones::LOWER_BODY:
			boneID = BONE_PELVIS;
			if (currentSetting.desiredMultiBones[i]) 
				PelvisHitbox(boneID);
			else 
				DefaultHitbox(boneID);
			break;
	}
}

void GetBestSpotAndDamage(C_BasePlayer* player,C_BasePlayer* localplayer, Vector& Spot, int& Damage,const RageWeapon_t& currSettings)
{	
	if (!player || !localplayer || !player->GetAlive() || !localplayer->GetAlive())
		return;
		
	// Atleast Now Total Bones we are caring of
	Vector spot = Vector(0);
	int damage = 0.f;

	int playerHelth = player->GetHealth();
	const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);

	// static matrix3x4_t boneMatrix[128];
	// 	player->SetupBones(boneMatrix, 128, 0x100, 0);
	
	static int i;
	for (i = 0; i < 6; i++)
	{
		GetDamageAndSpots(player, spot, damage, playerHelth, i, modelType, currSettings);
		if (damage >= playerHelth)
		{
			Damage = damage;
			Spot = spot;
			return;
		}
		else if (damage > Damage)
		{
			Damage = damage;
			Spot = spot;
		}
	}	
}

void RagebotNoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    if (!(cmd->buttons & IN_ATTACK) 
		|| !localplayer
		|| !localplayer->GetAlive())
		return;
	
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
		return;

	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	if (currentSettings.silent)
	{
		angle.x -= CurrentPunch.x * 2.f;
		angle.y -= CurrentPunch.y * 2.f;
	}
	else if (localplayer->GetShotsFired() > 1 )
	{
		QAngle NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * 2.0;
		angle.y -= NewPunch.y * 2.0;
	}
	RCSLastPunch = CurrentPunch;
}

void RagebotAutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    if (!localplayer || !localplayer->GetAlive() || !Settings::Ragebot::AutoCrouch::enable)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK;
}

void RagebotAutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, QAngle& angle, const RageWeapon_t& currentSettings)
{
	if (!currentSettings.autoSlow)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if ( !activeWeapon || activeWeapon->GetInReload())
		return;
	
	if (currentSettings.autoScopeEnabled && 
		Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && 
		!localplayer->IsScoped() && 
		!(cmd->buttons & IN_ATTACK2) && 
		!(cmd->buttons&IN_ATTACK))
		{ 
			cmd->buttons |= IN_ATTACK2; 
		}
		

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime ) 
		return;

	 QAngle ViewAngle;
	 	engine->GetViewAngles(ViewAngle);
	 static Vector oldOrigin = localplayer->GetAbsOrigin( );
	 Vector velocity = ( localplayer->GetVecOrigin( )-oldOrigin ) * (1.f/globalVars->interval_per_tick);
	 oldOrigin = localplayer->GetAbsOrigin( );
	 float speed  = velocity.Length( );

	 if(speed > 15.f)
	 {
	 	QAngle dir;
	 	Math::VectorAngles(velocity, dir);
	 	dir.y = ViewAngle.y - dir.x;
	 	Vector NewMove = Vector(0);
	 	Math::AngleVectors(dir, NewMove);
	 	auto max = std::max(forrwordMove, sideMove);
	 	auto mult = 450.f/max;
	 	NewMove *= -mult;
			
	 	forrwordMove = NewMove.x;
	 	sideMove = NewMove.y;
	 }
	 else 
	{	
		forrwordMove = 0.f;
		sideMove = 0.f;
	 }
	
	cmd->buttons |= IN_WALK;
}
void Ragebot::CheckHit(C_BaseCombatWeapon *activeWeapon){
        C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
        if (!localplayer || !localplayer->GetAlive())
                return;
        // cvar->ConsoleDPrintf("1 \n");
        if (!Ragebot::lockedEnemy.shooted || !Ragebot::lockedEnemy.player)
                return;
        // cvar->ConsoleDPrintf("2 \n");
    if (Ragebot::lockedEnemy.playerhelth != Ragebot::lockedEnemy.player->GetHealth() || !Ragebot::lockedEnemy.player->GetAlive())
                return;
        // cvar->ConsoleDPrintf("3 \n");
        // if ( activeWeapon->GetNextPrimaryAttack() > static_cast<float>(localplayer->GetTickBase()) * TICK_INTERVAL)
        //      return;

    Ray_t ray;
    trace_t tr;
        /*
    // traceStart = localplayer->GetEyePosition();
        // traceEnd = this->bullePosition;
        */\
        const Vector &traceStart = localplayer->GetEyePosition();
        const Vector &traceENd = *bulletPosition;
    ray.Init(traceStart, traceENd);
        CTraceFilter* traceFilter = new CTraceFilter();
    traceFilter->pSkip = localplayer;

    trace->TraceRay(ray, MASK_SHOT, traceFilter, &tr);

    // cvar->ConsoleDPrintf("traced \n");
    if (tr.m_pEntityHit == Ragebot::lockedEnemy.player){
        Resolver::players[Ragebot::lockedEnemy.player->GetIndex()].MissedCount++;

    cvar->ConsoleDPrintf("[eyehook] Missed shot due to bad resolve [RESOLVER: ");
    switch (Settings::Resolver::resolverType)
        {
        case resolverType::Experimental:
        if (!Settings::Resolver::manual){
        cvar->ConsoleDPrintf("EXP, STEP "); 
        cvar->ConsoleDPrintf(std::to_string(Resolver::players[Resolver::TargetID].MissedCount).c_str()); 
        }
        else if (!Settings::Resolver::forcebrute) {
                cvar->ConsoleDPrintf("MANUAL, Y: ");
                cvar->ConsoleDPrintf(std::to_string(Settings::Resolver::EyeAngles).c_str());
                cvar->ConsoleDPrintf(" , X: ");
                cvar->ConsoleDPrintf(std::to_string(Settings::Resolver::Pitch).c_str());
        }else if (Settings::Resolver::forcebrute && Settings::Resolver::manual){
     cvar->ConsoleDPrintf("BRUTEFORCE");}
        break;        
        case resolverType::ApuWare:
                        cvar->ConsoleDPrintf("ApuWare");

        break;

        }
cvar->ConsoleDPrintf("]\n");
}

        else{
cvar->ConsoleDPrintf("[eyehook] Missed shot due to spread\n");

        }

        if (Resolver::players[Ragebot::lockedEnemy.player->GetIndex()].MissedCount > 4){
                Resolver::players[Ragebot::lockedEnemy.player->GetIndex()].MissedCount = 0;
        }

        Ragebot::lockedEnemy.shooted = false;
        Ragebot::lockedEnemy.playerhelth = 0;
}


void RagebotAutoR8(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
{
    if (!currentSettings.autoShootEnabled)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if (!activeWeapon || activeWeapon->GetInReload())
		return;

	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
	{		
		cmd->buttons |= IN_ATTACK;

    	float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
		if (player)
		{
			if ( !Ragebot::ragebotPredictionSystem->canShoot(cmd, localplayer, activeWeapon, bestspot, player, currentSettings))
			{
				RagebotAutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle, currentSettings);
				cmd->buttons &= ~IN_ATTACK;
			}
		}
		else if (postponeFireReadyTime < globalVars->curtime )
			cmd->buttons &= ~IN_ATTACK;

		return;
	}
}

void RagebotAutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
{
    if (!currentSettings.autoShootEnabled)
		return;
	if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		return;
		
	if ( Ragebot::ragebotPredictionSystem->canShoot(cmd, localplayer, activeWeapon, bestspot, player, currentSettings) )
	{
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else if ( !(cmd->buttons & IN_ATTACK) )
			cmd->buttons |= IN_ATTACK;
		return;
	}

	RagebotAutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle, currentSettings);
}
void Ragebot::gotoStart(CUserCmd* cmd) {
C_BasePlayer* localPlayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localPlayer || localPlayer->GetDormant() || !localPlayer->GetAlive()) return;
    Vector playerLoc = localPlayer->GetAbsOrigin();

    float yaw = cmd->viewangles.y;
    Vector VecForward = playerLoc - quickpeekstartpos;

    Vector translatedVelocity = Vector{
        (float)(VecForward.x * cos(yaw / 180 * (float)M_PI) + VecForward.y * sin(yaw / 180 * (float)M_PI)),
        (float)(VecForward.y * cos(yaw / 180 * (float)M_PI) - VecForward.x * sin(yaw / 180 * (float)M_PI)),
        VecForward.z
    };
    cmd->forwardmove = -translatedVelocity.x * 20.f;
    cmd->sidemove = translatedVelocity.y * 20.f;
}
void Ragebot::quickpeek(CUserCmd* cmd) {
C_BasePlayer* localPlayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localPlayer || localPlayer->GetDormant() || !localPlayer->GetAlive()) return;
    if (inputSystem->IsButtonDown(Settings::Ragebot::quickpeek::key)) {
        if (quickpeekstartpos == Vector {0, 0, 0}) {
            quickpeekstartpos = localPlayer->GetAbsOrigin();
        }
        else {
            if (cmd->buttons & IN_ATTACK) hasShot = true;
            if (hasShot) {
                Ragebot::gotoStart(cmd);
            }
        }
    }
    else {
        hasShot = false;
        quickpeekstartpos = Vector{ 0, 0, 0 };
    }
}
void Ragebot::drawStartPos() {
    C_BasePlayer* localPlayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (quickpeekstartpos != Vector{ 0, 0, 0 }) {
	Vector spot2D;
	Vector playerPos;
debugOverlay->ScreenPosition( quickpeekstartpos, spot2D);
debugOverlay->ScreenPosition( localPlayer->GetAbsOrigin(), playerPos);
Draw::AddCircle3D(quickpeekstartpos, 32, Settings::Ragebot::quickpeek::color.Color(), 32);
//	Draw::AddCircleFilled( spot2D.x, spot2D.y, 32.0f, Settings::Ragebot::quickpeek::color.Color(), 32);
	Draw::AddLine( playerPos.x, playerPos.y, spot2D.x, spot2D.y, Settings::Ragebot::quickpeek::color.Color() ); 
}
}
static void FixMouseDeltas(CUserCmd* cmd, C_BasePlayer* player, QAngle& angle, QAngle& oldAngle)
{
    if (!player || !player->GetAlive())
		return;

    QAngle delta = angle - oldAngle;
    const float &sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
    const float &m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
    const float &m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
    const float &zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

    Math::NormalizeAngles(delta);

    cmd->mousedx = -delta.y / (m_yaw * sens * zoomMultiplier);
    cmd->mousedy = delta.x / (m_pitch * sens * zoomMultiplier);
}

C_BasePlayer* GetBestEnemyAndSpot(C_BasePlayer* localplayer,const RageWeapon_t& currSettings)
{
	using namespace Ragebot;

	if (!localplayer || !localplayer->GetAlive())
		return nullptr;
	
	Vector bestSpot = Vector(0);
	int bestDamage = 0;

	BestDamage = 0;
	BestSpot = Vector(0);

	if (lockedEnemy.player)
	{
		if (lockedEnemy.player->GetAlive() && !lockedEnemy.player->GetDormant() && !Ragebot::lockedEnemy.player->GetImmune())
		{
			GetBestSpotAndDamage(lockedEnemy.player,localplayer, bestSpot, bestDamage, currSettings);
			if (bestDamage >= lockedEnemy.player->GetHealth() || bestDamage >= currSettings.MinDamage || (Settings::Ragebot::mindmgoverride && inputSystem->IsButtonDown(Settings::Ragebot::dmgkey)))
			{
				BestDamage = bestDamage;
				BestSpot = bestSpot;
				return lockedEnemy.player;
			}
		}
		else 
		{
			lockedEnemy.player = nullptr;
			lockedEnemy.bestDamage = 0;
			lockedEnemy.lockedSpot = Vector(0);
		}
		
	}
	
	C_BasePlayer* clossestEnemy = nullptr;
	int maxClient = engine->GetMaxClients();
	for (int i = 1; i < maxClient; ++i)
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

		GetBestSpotAndDamage(player,localplayer, bestSpot, bestDamage, currSettings);
		
		if (bestDamage >= player->GetHealth() )
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

	if (BestDamage < currSettings.MinDamage || BestDamage <= 0)
		return nullptr;

	return clossestEnemy;
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
//Heres my ghetto attempt at this.
	if (Settings::Ragebot::impacttype == impactType::ITSME){
        if (Ragebot::lockedEnemy.shooted && lockedEnemy.player && activeWeapon->GetNextPrimaryAttack() <= static_cast<float>(localplayer->GetTickBase()) * TICK_INTERVAL && localplayer->GetAlive() && !(activeWeapon->GetInReload()))
        {
      Vector traceStart, traceEnd;

        traceStart = Ragebot::localEye;
        traceEnd.x = Ragebot::lockedEnemy.lockedSpot.x;
        traceEnd.y = Ragebot::lockedEnemy.lockedSpot.y;
        traceEnd.z = Ragebot::lockedEnemy.lockedSpot.z;
    Ray_t ray;
    trace_t tr;

    ray.Init(traceStart, traceEnd);
 CTraceFilter traceFilter;
                traceFilter.pSkip = localplayer;

    trace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);


                if (Ragebot::lockedEnemy.playerhelth == lockedEnemy.player->GetHealth() && lockedEnemy.player->GetAlive()){
    if (tr.m_pEntityHit == lockedEnemy.player){

                        Resolver::players[Resolver::TargetID].MissedCount++;
                                         cvar->ConsoleDPrintf("[eyehook] Missed shot due to bad resolve [RESOLVER: ");
    switch (Settings::Resolver::resolverType)
        {
        case resolverType::Experimental:
	if (!Settings::Resolver::manual){
	cvar->ConsoleDPrintf("EXP, STEP "); 
	cvar->ConsoleDPrintf(std::to_string(Resolver::players[Resolver::TargetID].MissedCount).c_str()); 
	}
	else if (!Settings::Resolver::forcebrute) {
	        cvar->ConsoleDPrintf("MANUAL, Y: ");
		cvar->ConsoleDPrintf(std::to_string(Settings::Resolver::EyeAngles).c_str());
                cvar->ConsoleDPrintf(" , X: ");
                cvar->ConsoleDPrintf(std::to_string(Settings::Resolver::Pitch).c_str());
	}else if (Settings::Resolver::forcebrute && Settings::Resolver::manual){
     cvar->ConsoleDPrintf("BRUTEFORCE");}
	break;        
	case resolverType::ApuWare:
	                cvar->ConsoleDPrintf("ApuWare");

	break;

	}
cvar->ConsoleDPrintf("]\n");
}
else{
cvar->ConsoleDPrintf("[eyehook] Missed shot due to spread\n");
}     
                   }

                if (Resolver::players[Resolver::TargetID].MissedCount > 4)
                        Resolver::players[Resolver::TargetID].MissedCount = 0;

                Ragebot::lockedEnemy.shooted = false;
                Ragebot::lockedEnemy.playerhelth = 0;
        }
}
else {
 Ragebot::CheckHit(activeWeapon);
}
	QAngle oldAngle;
    engine->GetViewAngles(oldAngle);
    
	float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
	QAngle angle = cmd->viewangles;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
    if (Settings::Ragebot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Ragebot::weapons.end())
		  index = *activeWeapon->GetItemDefinitionIndex();
    const RageWeapon_t &currentWeaponSetting = Settings::Ragebot::weapons.at(index);
		

	Ragebot::localEye = localplayer->GetEyePosition();
    Ragebot::BestSpot = Vector(0);
	Ragebot::BestDamage = 0;

	C_BasePlayer* player = nullptr;
	player = GetBestEnemyAndSpot(localplayer, currentWeaponSetting);

    if (player && Ragebot::BestDamage > 0)
    {
		Resolver::TargetID = player->GetIndex();
		Ragebot::lockedEnemy.player = player;
		Ragebot::lockedEnemy.lockedSpot = Ragebot::BestSpot;
		Ragebot::lockedEnemy.bestDamage = Ragebot::BestDamage;
		Settings::Debug::AutoAim::target = Ragebot::BestSpot;

		RagebotAutoShoot(player, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, oldForward, oldSideMove, currentWeaponSetting);
    	RagebotAutoR8(player, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, oldForward, oldSideMove, currentWeaponSetting);
		RagebotAutoCrouch(player, cmd, activeWeapon, currentWeaponSetting);

		// bf_write* buf;
		// CUserCmd* from = cmd;
		// CUserCmd* to = cmd;

		// to->command_number++;
		// to->tick_count +=  200;
		// WriteUserCmd(buf, from, to);

		if (cmd->buttons & IN_ATTACK)
		{
			angle = Math::CalcAngle(Ragebot::localEye, Ragebot::BestSpot);
			lockedEnemy.shooted = true;
			lockedEnemy.playerhelth = lockedEnemy.player->GetHealth();
			CreateMove::sendPacket = true;
		}
			
    }
	else{
		Ragebot::lockedEnemy.player = nullptr;
		Ragebot::lockedEnemy.lockedSpot = Vector(0);
		Ragebot::lockedEnemy.bestDamage = 0;
	}
	
	RagebotNoRecoil(angle, cmd, localplayer, activeWeapon, currentWeaponSetting);
	
    Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

    FixMouseDeltas(cmd, player, angle, oldAngle);
    cmd->viewangles = angle;

	if (!currentWeaponSetting.silent)
		engine->SetViewAngles(cmd->viewangles);

	Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
Ragebot::quickpeek(cmd);
Ragebot::drawStartPos();
}

void Ragebot::FireGameEvent(IGameEvent* event)
{
	if(!event)	
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
if ( strcmp(event->GetName(), XORSTR("bullet_impact")) == 0 && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) == engine->GetLocalPlayer()){
                float x = event->GetFloat(XORSTR("x")), y = event->GetFloat(XORSTR("y")), z = event->GetFloat(XORSTR("z"));
                bulletPosition->x = x; bulletPosition->y = y; bulletPosition->z = z; 
}

}

