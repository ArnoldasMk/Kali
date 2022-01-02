#include "triggerbot.h"
#include "autowall.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"

bool TriggercanShoot(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, const LegitWeapon_t& currentSettings)
{
	if(!localplayer || !localplayer->GetAlive() )
		return false;
	if (!activeWeapon || activeWeapon->GetInReload())
		return false;
	if (!currentSettings.hitchanceEnabled)
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
	
	return hitchance >= (currentSettings.hitchanceValue*2);
}

void Triggerbot::CreateMove(CUserCmd *cmd)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetAmmo() == 0)
		return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
		index = *activeWeapon->GetItemDefinitionIndex();
	const LegitWeapon_t& currentWeaponSetting = Settings::Legitbot::weapons.at(index);

	if (!currentWeaponSetting.TriggerBot)
		return;

	if (!inputSystem->IsButtonDown(Settings::Triggerbot::key))
		return;

	if ( !TriggercanShoot(localplayer, activeWeapon, currentWeaponSetting))
		return;

	if (Settings::Triggerbot::Filters::flashCheck && localplayer->IsFlashed())
		return;

	long currentTime_ms = Util::GetEpochTime();
	static long timeStamp = currentTime_ms;
	long oldTimeStamp;


	static int localMin = Settings::Triggerbot::RandomDelay::lowBound;
	static int localMax = Settings::Triggerbot::RandomDelay::highBound;
	static int randomDelay = localMin + rand() % (localMax - localMin);

	if( localMin != Settings::Triggerbot::RandomDelay::lowBound || localMax != Settings::Triggerbot::RandomDelay::highBound ) // Done in case Low/high bounds change before the next triggerbot shot.
	{
		localMin = Settings::Triggerbot::RandomDelay::lowBound;
		localMax = Settings::Triggerbot::RandomDelay::highBound;
		randomDelay = localMin + rand() % (localMax - localMin);
	}


	Vector traceStart, traceEnd;
	trace_t tr;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);
	QAngle viewAngles_rcs = viewAngles + *localplayer->GetAimPunchAngle() * 2.0f;

	Math::AngleVectors(viewAngles_rcs, traceEnd);

	traceStart = localplayer->GetEyePosition();
	traceEnd = traceStart + (traceEnd * 8192.0f);

	if (Settings::Triggerbot::Filters::walls)
	{
		AutoWall::FireBulletData data;
		if (AutoWall::GetDamage(traceEnd, !Settings::Triggerbot::Filters::allies, data) == 0.0f)
			return;

		tr = data.enter_trace;
	}
	else
	{
		Ray_t ray;
		ray.Init(traceStart, traceEnd);
		CTraceFilter traceFilter;
		traceFilter.pSkip = localplayer;
		trace->TraceRay(ray, 0x46004003, &traceFilter, &tr);
	}

	oldTimeStamp = timeStamp;
	timeStamp = currentTime_ms;

	C_BasePlayer* player = (C_BasePlayer*) tr.m_pEntityHit;
	if (!player)
		return;

	if (player->GetClientClass()->m_ClassID != EClassIds::CCSPlayer)
		return;

	if (player == localplayer
		|| player->GetDormant()
		|| !player->GetAlive()
		|| player->GetImmune())
		return;

	if (!Entity::IsTeamMate(player, localplayer) && !Settings::Triggerbot::Filters::enemies)
		return;

	if (Entity::IsTeamMate(player, localplayer) && !Settings::Triggerbot::Filters::allies)
		return;

	bool filter;

	switch (tr.hitgroup)
	{
		case HitGroups::HITGROUP_HEAD:
			filter = Settings::Triggerbot::Filters::head;
			break;
		case HitGroups::HITGROUP_CHEST:
			filter = Settings::Triggerbot::Filters::chest;
			break;
		case HitGroups::HITGROUP_STOMACH:
			filter = Settings::Triggerbot::Filters::stomach;
			break;
		case HitGroups::HITGROUP_LEFTARM:
		case HitGroups::HITGROUP_RIGHTARM:
			filter = Settings::Triggerbot::Filters::arms;
			break;
		case HitGroups::HITGROUP_LEFTLEG:
		case HitGroups::HITGROUP_RIGHTLEG:
			filter = Settings::Triggerbot::Filters::legs;
			break;
		default:
			filter = false;
	}

	if (!filter)
		return;

	if (Settings::Triggerbot::Filters::smokeCheck && LineGoesThroughSmoke(tr.startpos, tr.endpos, 1))
		return;

	ItemDefinitionIndex itemDefinitionIndex = *activeWeapon->GetItemDefinitionIndex();
	if (itemDefinitionIndex == ItemDefinitionIndex::WEAPON_KNIFE || itemDefinitionIndex >= ItemDefinitionIndex::WEAPON_KNIFE_BAYONET)
		return;
	if (Settings::Legitbot::AutoShoot::autoscope)
	    if (Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) )
	    {
			cmd->buttons |= IN_ATTACK2;
			return; // will go to the next tick
	    }
	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
	{
		if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
			cmd->buttons &= ~IN_ATTACK2;
		else
			cmd->buttons &= ~IN_ATTACK;
	}
	else
	{
		if (Settings::Triggerbot::RandomDelay::enabled && currentTime_ms - oldTimeStamp < randomDelay)
		{
			timeStamp = oldTimeStamp;
			return;
		}

		if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
			cmd->buttons |= IN_ATTACK2;
		else if ( !(cmd->buttons & IN_ATTACK) )
			cmd->buttons |= IN_ATTACK;
		if(Settings::Triggerbot::RandomDelay::enabled)
			Settings::Triggerbot::RandomDelay::lastRoll = randomDelay;

		randomDelay = localMin + rand() % (localMax - localMin);
	}
	timeStamp = currentTime_ms;
}
