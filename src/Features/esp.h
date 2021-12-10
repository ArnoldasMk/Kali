	#pragma once

#include "../ImGUI/imgui.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/vector.h"
#include "../SDK/IVModelRender.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"


namespace ESP
{
	extern const char* ranks[];

	ImColor GetESPPlayerColor(C_BasePlayer* player, bool visible);
	bool WorldToScreen( const Vector &origin, ImVec2 * const screen );
                                const std::map<ItemDefinitionIndex, std::string> Weaponsi = {
{ ItemDefinitionIndex::WEAPON_DEAGLE, "A" },
{ ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, "4" },
{ ItemDefinitionIndex::WEAPON_DECOY,"m" },
{ ItemDefinitionIndex::WEAPON_AUG,"U" },
{ ItemDefinitionIndex::WEAPON_G3SG1,"X" },
{ ItemDefinitionIndex::WEAPON_MAC10,"K" },
{ ItemDefinitionIndex::WEAPON_P90,"P" },
{ ItemDefinitionIndex::WEAPON_SSG08,"a" },
{ ItemDefinitionIndex::WEAPON_SCAR20,"Y" },
{ ItemDefinitionIndex::WEAPON_UMP45,"L" },
{ ItemDefinitionIndex::WEAPON_ELITE,"B" },
{ ItemDefinitionIndex::WEAPON_FAMAS,"R" },
{ ItemDefinitionIndex::WEAPON_FIVESEVEN,"C" },
{ ItemDefinitionIndex::WEAPON_GALILAR,"Q" },
{ ItemDefinitionIndex::WEAPON_M4A1_SILENCER,"T" },
{ ItemDefinitionIndex::WEAPON_M4A1,"S" },
{ ItemDefinitionIndex::WEAPON_P250,"F" },
{ ItemDefinitionIndex::WEAPON_M249,"g" },
{ ItemDefinitionIndex::WEAPON_XM1014,"b" },
{ ItemDefinitionIndex::WEAPON_GLOCK,"D" },
{ ItemDefinitionIndex::WEAPON_USP_SILENCER,"G" },
{ ItemDefinitionIndex::WEAPON_HKP2000,"E" },
{ ItemDefinitionIndex::WEAPON_AK47,"W" },
{ ItemDefinitionIndex::WEAPON_AWP,"Z" },
{ ItemDefinitionIndex::WEAPON_BIZON,"M" },
{ ItemDefinitionIndex::WEAPON_MAG7,"d" },
{ ItemDefinitionIndex::WEAPON_NEGEV,"f" },
{ ItemDefinitionIndex::WEAPON_SAWEDOFF,"c" },
{ ItemDefinitionIndex::WEAPON_TEC9,"H" },
{ ItemDefinitionIndex::WEAPON_TASER,"h" },
{ ItemDefinitionIndex::WEAPON_NOVA,"e" },
{ ItemDefinitionIndex::WEAPON_CZ75A,"I" },
{ ItemDefinitionIndex::WEAPON_SG556,"V" },
{ ItemDefinitionIndex::WEAPON_REVOLVER,"J" },
{ ItemDefinitionIndex::WEAPON_MP7,"N" },
{ ItemDefinitionIndex::WEAPON_MP9,"O" },
{ ItemDefinitionIndex::WEAPON_MP5,"L" },
{ ItemDefinitionIndex::WEAPON_C4,"o" },
{ ItemDefinitionIndex::WEAPON_FRAG_GRENADE,"j" },
{ ItemDefinitionIndex::WEAPON_SMOKEGRENADE,"k" },
{ ItemDefinitionIndex::WEAPON_MOLOTOV,"l" },
{ ItemDefinitionIndex::WEAPON_INCGRENADE,"n" },
{ ItemDefinitionIndex::WEAPON_FLASHBANG,"i" },
{ ItemDefinitionIndex::WEAPON_KNIFE,"5" },
                };

	//Hooks
	void DrawModelExecute();
	bool PrePaintTraverse(VPANEL vgui_panel, bool force_repaint, bool allow_force);
	void Paint();
	void PaintToUpdateMatrix();
	void CreateMove(CUserCmd* cmd);
	void FireGameEvent(IGameEvent* event);

}
