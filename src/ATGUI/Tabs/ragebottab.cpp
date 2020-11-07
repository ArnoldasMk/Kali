#include "legitbottab.h"
#include "ragebottab.h"

#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../../Hacks/ragebot.h"
#include "../../Utils/ColorPickerButton.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;
static DamagePrediction damagePrediction = DamagePrediction::justDamage;
static EnemySelectionType enemySelectionType = EnemySelectionType::BestDamage;

//static bool enabled = false;
static bool silent = false;
static bool friendly = false;
static bool closestBone = false;
static bool desireBones[] = {true, true, true, true, true, true};
static bool desiredMultiBones[] = {true, true, true, true, true, true};
static bool autoPistolEnabled = false;
static bool autoShootEnabled = false;
static bool autoScopeEnabled = false;
static bool HitChanceEnabled = false;
static float HitChange = 20.f;
static float MinDamage = 50.f;
static bool autoSlow = false;
static bool doubleFire = false;
static bool scopeControlEnabled = false;


void UI::ReloadRageWeaponSettings()
{
	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Ragebot::weapons.find(currentWeapon) != Settings::Ragebot::weapons.end())
		index = currentWeapon;

	silent = Settings::Ragebot::weapons.at(index).silent;
	friendly = Settings::Ragebot::weapons.at(index).friendly;
	closestBone = Settings::Ragebot::weapons.at(index).closestBone;
	autoPistolEnabled = Settings::Ragebot::weapons.at(index).autoPistolEnabled;
	autoShootEnabled = Settings::Ragebot::weapons.at(index).autoShootEnabled;
	autoScopeEnabled = Settings::Ragebot::weapons.at(index).autoScopeEnabled;
	HitChanceEnabled = Settings::Ragebot::weapons.at(index).HitChanceEnabled;
	HitChange = Settings::Ragebot::weapons.at(index).HitChance;
	MinDamage = Settings::Ragebot::weapons.at(index).MinDamage;
	autoSlow = Settings::Ragebot::weapons.at(index).autoSlow;
	doubleFire = Settings::Ragebot::weapons.at(index).DoubleFire;
	scopeControlEnabled = Settings::Ragebot::weapons.at(index).scopeControlEnabled;
	damagePrediction = Settings::Ragebot::weapons.at(index).DmagePredictionType;
	enemySelectionType = Settings::Ragebot::weapons.at(index).enemySelectionType;

	for (int BONE = 0; BONE < 6; BONE++)
	{
		desireBones[BONE] = Settings::Ragebot::weapons.at(index).desireBones[BONE];
		desiredMultiBones[BONE] = Settings::Ragebot::weapons.at(index).desiredMultiBones[BONE];
	}
		

	// Ragebot::UpdateValues();
}

void UI::UpdateRageWeaponSettings()
{
	if (Settings::Ragebot::weapons.find(currentWeapon) == Settings::Ragebot::weapons.end() && Settings::Ragebot::enabled)
		Settings::Ragebot::weapons[currentWeapon] = RageWeapon_t();

	RageWeapon_t settings = {
			.silent = silent,
			.friendly = friendly,
			.closestBone = closestBone,
			.HitChanceEnabled = HitChanceEnabled,	
			.autoPistolEnabled = autoPistolEnabled,
			.autoShootEnabled = autoShootEnabled,
			.autoScopeEnabled = autoScopeEnabled,
			.autoSlow = autoSlow,
			.scopeControlEnabled = scopeControlEnabled,
			.DoubleFire = doubleFire,
			.MinDamage = MinDamage,
			.HitChance = HitChange,
			.DmagePredictionType = damagePrediction,
			.enemySelectionType = enemySelectionType,	
	};


	for (int BONE = 0; BONE < 6; BONE++){
		settings.desireBones[BONE] = desireBones[BONE];
		settings.desiredMultiBones[BONE] = desiredMultiBones[BONE];
	}
		

	Settings::Ragebot::weapons.at(currentWeapon) = settings;

	if (Settings::Ragebot::weapons.at(currentWeapon) == Settings::Ragebot::weapons.at(ItemDefinitionIndex::INVALID) &&
		currentWeapon != ItemDefinitionIndex::INVALID)
	{
		Settings::Ragebot::weapons.erase(currentWeapon);
		UI::ReloadRageWeaponSettings();
		return;
	}

	// Ragebot::UpdateValues();
}

void RagebotTab::RenderTab()
{
	static char filterWeapons[32];

	
	const char *DamagePredictionType[] = {"Safety","Damage",};
	const char *EnemySelectionType[] = {"Best Damage(Lagacy Old Method)", "Closest To Crosshair( Faster But In alfa)"};
        const char *impactType[] = {"itsme","Pritam",};


	
	ImGui::Columns(3, nullptr, false);
	{
		ImGui::SetColumnOffset(1, 200);
		ImGui::PushItemWidth(-10);

		if (ImGui::Checkbox(XORSTR("Enabled"), &Settings::Ragebot::enabled))
		{	
			Settings::Legitbot::enabled = false;
			UI::UpdateRageWeaponSettings();
		}

		ImGui::InputText(XORSTR("##FILTERWEAPONS"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
		ImGui::PopItemWidth();
		ImGui::ListBoxHeader(XORSTR("##GUNS"), ImVec2(-1, -1));
		for (auto it : ItemDefinitionIndexMap)
		{
			bool isDefault = (int) it.first < 0;
			if (!isDefault && !Util::Contains(Util::ToLower(std::string(filterWeapons)), Util::ToLower(Util::Items::GetItemDisplayName(it.first).c_str())))
				continue;

			if (Util::Items::IsKnife(it.first) || Util::Items::IsGlove(it.first) || Util::Items::IsUtility(it.first))
				continue;

			const bool item_selected = ((int) it.first == (int) currentWeapon);
			ImGui::PushID((int) it.first);
			std::string formattedName;
			char changeIndicator = ' ';
			bool isChanged = Settings::Ragebot::weapons.find(it.first) != Settings::Ragebot::weapons.end();
			if (!isDefault && isChanged)
				changeIndicator = '*';
			formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str() : Util::Items::GetItemDisplayName(it.first));
			if (ImGui::Selectable(formattedName.c_str(), item_selected))
			{
				currentWeapon = it.first;
				UI::ReloadRageWeaponSettings();
			}
			ImGui::PopID();
		}
		ImGui::ListBoxFooter();
	}
	ImGui::NextColumn();
	{
		// !0 times Spacing Who want to write 10 ImGui::Spacing ?? :P
		for (int i = 0; i < 6; i++)
			ImGui::Spacing();

		ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 75);
		ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), false);
		{
			// Starting of Auto Shoot Features
			ImGui::Spacing(); ImGui::Spacing();
			ImGui::Columns(1, nullptr, false);
			{
				if (ImGui::Checkbox(XORSTR("Auto Shoot"), &autoShootEnabled))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Checkbox(XORSTR("AutoSlow"), &autoSlow))
					UI::UpdateRageWeaponSettings();
				if( ImGui::Checkbox(XORSTR("Hit Chance"), &HitChanceEnabled) )
					UI::UpdateRageWeaponSettings();
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				if( ImGui::SliderFloat(XORSTR("##HITCHANCE"), &HitChange, 1, 100, XORSTR("Percent %0.0f")) )
					UI::UpdateRageWeaponSettings();
				ImGui::PopItemWidth();

			}
			// END Auto Shoot Features
			ImGui::Spacing();
			// BONE SELECTION
			ImGui::PushItemWidth(-1);
			if ( ImGui::BeginCombo(XORSTR("##BONESELECTION"), XORSTR("SELECT BONES")) )
			{
				
				if ( ImGui::Selectable(XORSTR("HEAD"), &desireBones[(int)DesireBones::BONE_HEAD], ImGuiSelectableFlags_DontClosePopups) )
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("UPPER CHEST"), &desireBones[(int)DesireBones::UPPER_CHEST], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("MIDDLE CHEST"), &desireBones[(int)DesireBones::MIDDLE_CHEST], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("LOWER CHEST"), &desireBones[(int)DesireBones::LOWER_CHEST], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("HIP"), &desireBones[(int)DesireBones::BONE_HIP], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("LOWER BODY"), &desireBones[(int)DesireBones::LOWER_BODY], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				
				ImGui::EndCombo();
			}
			if ( ImGui::BeginCombo(XORSTR("##MultiBONESELECTION"), XORSTR("SELECT Multi Points")) )
			{
				if ( ImGui::Selectable(XORSTR("HEAD"), &desiredMultiBones[(int)DesireBones::BONE_HEAD], ImGuiSelectableFlags_DontClosePopups) )
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("UPPER CHEST"), &desiredMultiBones[(int)DesireBones::UPPER_CHEST], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("MIDDLE CHEST"), &desiredMultiBones[(int)DesireBones::MIDDLE_CHEST], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("LOWER CHEST"), &desiredMultiBones[(int)DesireBones::LOWER_CHEST], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("HIP"), &desiredMultiBones[(int)DesireBones::BONE_HIP], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				if (ImGui::Selectable(XORSTR("LOWER BODY"), &desiredMultiBones[(int)DesireBones::LOWER_BODY], ImGuiSelectableFlags_DontClosePopups))
					UI::UpdateRageWeaponSettings();
				
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			// END BONE SELECTION
			// Damage Prediction type
			ImGui::Spacing(); ImGui::Spacing();
			ImGui::Columns(1, nullptr, false);
			{
				ImGui::TextWrapped(XORSTR("Damage Prediction"));
				ImGui::PushItemWidth(-1);
				if(ImGui::Combo(XORSTR("##PredictionSystem"), (int*)&damagePrediction, DamagePredictionType, IM_ARRAYSIZE(DamagePredictionType) ) )
					UI::UpdateRageWeaponSettings();
				ImGui::PopItemWidth();
			}			
			// END Damage Prediction Type

			// Enemy Selection type
			ImGui::Spacing(); ImGui::Spacing();
			ImGui::Columns(1);
			{
				ImGui::TextWrapped(XORSTR("Enemy Selection"));
				ImGui::PushItemWidth(-1);
				if(ImGui::Combo(XORSTR("##SelectionSystem"), (int*)&enemySelectionType, EnemySelectionType, IM_ARRAYSIZE(EnemySelectionType) ) )
					UI::UpdateRageWeaponSettings();
				ImGui::PopItemWidth();
			}			
			// END Enemy Selection Type		
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		// Six Times Spacing Who want to write spacing for six time xd
		for (int i = 0; i < 6; i++)
			ImGui::Spacing();

		ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), false);
		{
			// Min damage Slider Settings
			ImGui::Columns(1);
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat(XORSTR("##VISIBLEDMG"), &MinDamage, 0, 150, XORSTR("Min Damage: %.0f")))
					UI::UpdateRageWeaponSettings();
				ImGui::PopItemWidth();
			}
			// END Min Dmaage Slider Settings

			/*
			* Resolver Tab whcih is only usefull in hvh moments so .... 
			* I transfer it to under rafe bot from anti aim tab
			*/
			ImGui::Spacing(); ImGui::Spacing();
			ImGui::Columns(1, nullptr, false);
		  const char* resolverType[] = {
		        "NONE",
		        "Experimental",
		        "ApuWare",
			"EXP (LegitAA)",
			"EXP (Rage)",
		    };
	                                ImGui::TextWrapped(XORSTR("Resolver Type"));

        ImGui::Combo(XORSTR("##ResolverType"), (int*)&Settings::Resolver::resolverType, resolverType, IM_ARRAYSIZE(resolverType));
        switch (Settings::Resolver::resolverType)
        {
        case resolverType::Experimental:
		Settings::Resolver::resolveAll = true;
		Settings::Resolver::resolveAllAP = false;
            break;
        case resolverType::ApuWare:
                Settings::Resolver::resolveAll = false;
                Settings::Resolver::resolveAllAP = true;
            break;
        case resolverType::Legit:
                Settings::Resolver::resolveAll = true;
                Settings::Resolver::resolveAllAP = false;
            break;
        case resolverType::Rage:
                Settings::Resolver::resolveAll = true;
                Settings::Resolver::resolveAllAP = false;
            break;
        case resolverType::NONE:
                Settings::Resolver::resolveAll = false;
                Settings::Resolver::resolveAllAP = false;

            break;
        default:
            break;
        }

                                        ImGui::TextWrapped(XORSTR("Miss Detection Type"));

ImGui::Combo(XORSTR("##ImpactDetection"), (int*)&Settings::Ragebot::impacttype, impactType, IM_ARRAYSIZE(impactType) );
                                ImGui::Checkbox(XORSTR("Override Minimun Damage"), &Settings::Ragebot::mindmgoverride);
                                UI::KeyBindButton(&Settings::Ragebot::dmgkey);
                                ImGui::Checkbox(XORSTR("Wait for onshot"), &Settings::Ragebot::onshot::enabled);
                                UI::KeyBindButton(&Settings::Ragebot::onshot::button);

			//	ImGui::Checkbox(XORSTR("Resolver"), &Settings::Resolver::resolveAll);


			// End of resolver tab

			// Others Settings For Weapons
			ImGui::Columns(1, nullptr, false);
			{

				switch (currentWeapon)
				{
					case ItemDefinitionIndex::INVALID:
					case ItemDefinitionIndex::WEAPON_DEAGLE:
					case ItemDefinitionIndex::WEAPON_ELITE:
					case ItemDefinitionIndex::WEAPON_FIVESEVEN:
					case ItemDefinitionIndex::WEAPON_GLOCK:
					case ItemDefinitionIndex::WEAPON_TEC9:
					case ItemDefinitionIndex::WEAPON_HKP2000:
					case ItemDefinitionIndex::WEAPON_USP_SILENCER:
					case ItemDefinitionIndex::WEAPON_P250:
					case ItemDefinitionIndex::WEAPON_CZ75A:
					case ItemDefinitionIndex::WEAPON_REVOLVER:
						if (ImGui::Checkbox(XORSTR("Auto Pistol"), &autoPistolEnabled))
							UI::UpdateRageWeaponSettings();
						break;
					default:
						break;
				}
				/*
				if (ImGui::Checkbox(XORSTR("Double Fire"), &doubleFire))
					UI::UpdateRageWeaponSettings();
				*/

				switch (currentWeapon)
				{
					case ItemDefinitionIndex::WEAPON_DEAGLE:
					case ItemDefinitionIndex::WEAPON_ELITE:
					case ItemDefinitionIndex::WEAPON_FIVESEVEN:
					case ItemDefinitionIndex::WEAPON_GLOCK:
					case ItemDefinitionIndex::WEAPON_TEC9:
					case ItemDefinitionIndex::WEAPON_HKP2000:
					case ItemDefinitionIndex::WEAPON_USP_SILENCER:
					case ItemDefinitionIndex::WEAPON_P250:
					case ItemDefinitionIndex::WEAPON_CZ75A:
					case ItemDefinitionIndex::WEAPON_REVOLVER:
						break;
					default:
						if (ImGui::Checkbox(XORSTR("Auto Scope"), &autoScopeEnabled))
							UI::UpdateRageWeaponSettings();
						// if (ImGui::Checkbox(XORSTR("Scope Control"), &scopeControlEnabled))
						// 	UI::UpdateRageWeaponSettings();
				}
					
				ImGui::Checkbox(XORSTR("Auto Crouch"), &Settings::Ragebot::AutoCrouch::enable);
				ImGui::Checkbox(XORSTR("Backtrack"), &Settings::Ragebot::backTrack::enabled);
				float time;
				if (Settings::SvCheats::fakelat)
					time = 0.4f;
				else
					time = 0.2f;
				if (Settings::Ragebot::backTrack::enabled){
                                ImGui::SliderFloat(XORSTR("##TRACKTIME"), &Settings::Ragebot::backTrack::time, 0.0f, time);
				if ( Settings::Ragebot::backTrack::time > time)
				Settings::Ragebot::backTrack::time = 0.2f;
				}

                                ImGui::Checkbox(XORSTR("AutoKnife"), &Settings::AutoKnife::enabled);
//                                ImGui::Checkbox(XORSTR("Exploits"), &Settings::Ragebot::exploits::enabled);
//				if (Settings::Ragebot::exploits::enabled){
  //                              ImGui::Checkbox(XORSTR("DoubleTap"), &Settings::Ragebot::exploits::doubletap);
//				    UI::KeyBindButton(&Settings::Ragebot::exploits::doubletapKey);

//				}
				if (ImGui::Checkbox(XORSTR("Silent Aim"), &silent))

					UI::UpdateRageWeaponSettings();
			}
			// END of other Settings
			
			
			//ImGui::ColorButton(XORSTR("MainColor"), Settings::UI::mainColor.ColorVec4, ImGuiColorEditFlags__DataTypeMask, ImVec2(15,15));
			
			ImGui::Columns(1, nullptr, false);
			ImGui::Spacing(); ImGui::Spacing();
			if (currentWeapon > ItemDefinitionIndex::INVALID && Settings::Ragebot::weapons.find(currentWeapon) != Settings::Ragebot::weapons.end())
			{
				if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0)))
				{
					Settings::Ragebot::weapons.erase(currentWeapon);
					UI::ReloadRageWeaponSettings();
				}
			}
			ImGui::EndChild();
		}
	}
}
