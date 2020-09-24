#include "quickswitch.h"

#include "../settings.h"
#include "../interfaces.h"


void QuickSwitch::CreateMove(CUserCmd *cmd)
{
	if (!Settings::QuickSwitch::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if (cmd->buttons & IN_ATTACK && localplayer->IsScoped())
	{
		C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
		if (!activeWeapon || activeWeapon->GetInReload())
			return;

		if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
		{
			engine->ClientCmd_Unrestricted("use weapon_knife;slot1"); // ghetto method
			return;
		}
	}
}
