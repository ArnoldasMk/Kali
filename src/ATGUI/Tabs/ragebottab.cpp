#include "legitbottab.h"
#include "ragebottab.h"

#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../../Features/ragebot.h"
#include "../../Utils/ColorPickerButton.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;
static DamagePrediction damagePrediction = DamagePrediction::justDamage;
static EnemySelectionType enemySelectionType = EnemySelectionType::BestDamage;

// static bool enabled = false;
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
static float BodyScale = 0.1f;
static float HeadScale = 0.1f;
static bool autoSlow = false;
static bool scopeControlEnabled = false;
static ButtonCode_t onshotkey = ButtonCode_t::KEY_3;
static ButtonCode_t mindmgoveridekey = ButtonCode_t::KEY_5;

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
	scopeControlEnabled = Settings::Ragebot::weapons.at(index).scopeControlEnabled;
	damagePrediction = Settings::Ragebot::weapons.at(index).DmagePredictionType;
	enemySelectionType = Settings::Ragebot::weapons.at(index).enemySelectionType;
	MinDamage = Settings::Ragebot::weapons.at(index).MinDamage;
	BodyScale = Settings::Ragebot::weapons.at(index).BodyScale;
	HeadScale = Settings::Ragebot::weapons.at(index).HeadScale;
	onshotkey = Settings::Ragebot::weapons.at(index).onshotkey;
	mindmgoveridekey = Settings::Ragebot::weapons.at(index).mindmgoveridekey;
	for (int BONE = 0; BONE < 6; BONE++)
	{
		desireBones[BONE] = Settings::Ragebot::weapons.at(index).desireBones[BONE];
		desiredMultiBones[BONE] = Settings::Ragebot::weapons.at(index).desiredMultiBones[BONE];
	}
}

void UI::UpdateRageWeaponSettings()
{

	if (Settings::Ragebot::weapons.find(currentWeapon) == Settings::Ragebot::weapons.end())
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

	    .MinDamage = MinDamage,
	    .HitChance = HitChange,
	    .BodyScale = BodyScale,
	    .HeadScale = HeadScale,
	    .onshotkey = onshotkey,
	    .mindmgoveridekey = mindmgoveridekey,

	    .DmagePredictionType = damagePrediction,
	    .enemySelectionType = enemySelectionType,
	};

	for (int BONE = 0; BONE < 6; BONE++)
	{
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
}

void RagebotTab::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	static char filterWeapons[32];
	const char *DamagePredictionType[] = {
	    "Safety",
	    "Damage",
	};
	const char *EnemySelectionType[] = {"Best Damage(Legacy Old Method)", "Closest To Crosshair( Faster But In alfa)"};
	const char *impactType[] = {
	    "itsme",
	    "Pritam",
	};

	draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 960 - 15, pos.y + 95), ImColor(0, 0, 0, 25), 10);
	ImGui::SetCursorPos(ImVec2(15, 70));
	ImGui::BeginGroup();
	{
		if (ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Ragebot::enabled))
		{
			Settings::Legitbot::enabled = false;
			UI::UpdateWeaponSettings();
		}
	}
	ImGui::EndGroup();
	ImGui::BeginGroup();
	{
		ImGui::SetCursorPos(ImVec2(10, 100));
		ImGui::Columns(3, nullptr, false);
		{
			ImGui::SetColumnOffset(1, 200);
			ImGui::PushItemWidth(-1);
			{
				ImGui::InputText(XORSTR("##FILTERWEAPONS"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
			}
			ImGui::PopItemWidth();
			ImGui::ListBoxHeader(XORSTR("##GUNS"), ImVec2(-1, 706));
			for (auto it : ItemDefinitionIndexMap)
			{
				bool isDefault = (int)it.first < 0;
				if (!isDefault && !Util::Contains(Util::ToLower(std::string(filterWeapons)),
										    Util::ToLower(Util::Items::GetItemDisplayName(it.first).c_str())))
				{
					continue;
				}
				if (Util::Items::IsKnife(it.first) || Util::Items::IsGlove(it.first) ||
				    Util::Items::IsUtility(it.first))
				{
					continue;
				}
				const bool item_selected = ((int)it.first == (int)currentWeapon);
				ImGui::PushID((int)it.first);
				std::string formattedName;
				char changeIndicator = ' ';
				bool isChanged = Settings::Ragebot::weapons.find(it.first) != Settings::Ragebot::weapons.end();
				if (!isDefault && isChanged)
				{
					changeIndicator = '*';
				}
				formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str()
													: Util::Items::GetItemDisplayName(it.first));
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
			ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 226);
			ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 736), true);
			{
				ImGui::Separator();
				ImGui::Text(XORSTR("Main Features"));
				ImGui::Separator();
				ImGui::Columns(1, nullptr, false);
				{
					if (ImGui::Checkbox(XORSTR("Auto Shoot"), &autoShootEnabled))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("AutoSlow"), &autoSlow))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Hit Chance"), &HitChanceEnabled))
						UI::UpdateRageWeaponSettings();
					ImGui::SameLine();
					ImGui::PushItemWidth(-1);
					if (ImGui::SliderFloat(XORSTR("##HITCHANCE"), &HitChange, 1, 100, XORSTR("Percent %0.0f")))
						UI::UpdateRageWeaponSettings();
					ImGui::PopItemWidth();
				}

				ImGui::Columns(1);
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::SliderFloat(XORSTR("##VISIBLEDMG"), &MinDamage, 0, 150, XORSTR("Min Damage: %.0f")))
						UI::UpdateRageWeaponSettings();
					ImGui::PopItemWidth();
				}
				ImGui::Separator();
				ImGui::Text(XORSTR("Resolver"));
				ImGui::Separator();
				ImGui::Columns(1, nullptr, false);
				const char *resolverType[] = {
				    "NONE",
				    "Experimental",
				    "ApuWare",
				    "EXP (LegitAA)",
				    "EXP (Rage)",
				};
				ImGui::TextWrapped(XORSTR("Resolver Type"));

				ImGui::Combo(XORSTR("##ResolverType"), (int *)&Settings::Resolver::resolverType, resolverType, IM_ARRAYSIZE(resolverType));
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
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text(XORSTR("Misc"));
				ImGui::Separator();
				ImGui::Columns(1);

				ImGui::Text(XORSTR("Miss Detection Type"));
				ImGui::Columns(1);
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::Combo(XORSTR("##ImpactDetection"), (int *)&Settings::Ragebot::impacttype, impactType, IM_ARRAYSIZE(impactType)))
						UI::UpdateRageWeaponSettings();
					ImGui::PopItemWidth();
				}
				ImGui::Text(XORSTR("Enemy Selection"));
				ImGui::Columns(1);
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::Combo(XORSTR("##SelectionSystem"), (int *)&enemySelectionType, EnemySelectionType, IM_ARRAYSIZE(EnemySelectionType)))
						UI::UpdateRageWeaponSettings();
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);
				ImGui::Columns(2, nullptr, true);
				{
					ImGui::PushItemWidth(-1);
					ImGui::Checkbox(XORSTR("Override Minimun Damage"), &Settings::Ragebot::mindmgoverride);
					ImGui::Checkbox(XORSTR("Wait for onshot"), &Settings::Ragebot::onshot::enabled);
					ImGui::PopItemWidth();
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					UI::KeyBindButton(&mindmgoveridekey);
					UI::KeyBindButton(&onshotkey);
					ImGui::PopItemWidth();
				}
				ImGui::Separator();
			}
			ImGui::EndChild();
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 736), true);
			{
				ImGui::Separator();
				ImGui::Text(XORSTR("Scaling"));
				ImGui::Separator();
				ImGui::Text(XORSTR("Head multipoint scale"));
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##HEADSCALE"), &HeadScale, 0.0f, 1.0f);
				ImGui::PushItemWidth(-1);
				ImGui::Text(XORSTR("Body multipoint scale"));
				ImGui::SliderFloat(XORSTR("##BODYSCALE"), &BodyScale, 0.0f, 1.0f);
				ImGui::Separator();

				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Text(XORSTR("Target Bones"));
				}
				ImGui::NextColumn();
				{
					ImGui::Text(XORSTR("Target MultiPoints"));
				}
				ImGui::EndColumns();
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					if (ImGui::Checkbox(XORSTR("HEAD"), &desireBones[(int)DesireBones::BONE_HEAD]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("UPPER CHEST"), &desireBones[(int)DesireBones::UPPER_CHEST]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("MIDDLE CHEST"), &desireBones[(int)DesireBones::MIDDLE_CHEST]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("LOWER CHEST"), &desireBones[(int)DesireBones::LOWER_CHEST]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("HIP"), &desireBones[(int)DesireBones::BONE_HIP]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("LOWER BODY"), &desireBones[(int)DesireBones::LOWER_BODY]))
						UI::UpdateRageWeaponSettings();
				}
				ImGui::NextColumn();
				{
					if (ImGui::Checkbox(XORSTR("HEAD"), &desiredMultiBones[(int)DesireBones::BONE_HEAD]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("UPPER CHEST"), &desiredMultiBones[(int)DesireBones::UPPER_CHEST]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("MIDDLE CHEST"), &desiredMultiBones[(int)DesireBones::MIDDLE_CHEST]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("LOWER CHEST"), &desiredMultiBones[(int)DesireBones::LOWER_CHEST]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("HIP"), &desiredMultiBones[(int)DesireBones::BONE_HIP]))
						UI::UpdateRageWeaponSettings();
					if (ImGui::Checkbox(XORSTR("LOWER BODY"), &desiredMultiBones[(int)DesireBones::LOWER_BODY]))
						UI::UpdateRageWeaponSettings();
				}
				ImGui::EndColumns();
				ImGui::Separator();
				ImGui::Text(XORSTR("Other Features"));
				ImGui::Separator();
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
						if (ImGui::Checkbox(XORSTR("Scope Control"), &scopeControlEnabled))
							UI::UpdateRageWeaponSettings();
					}

					ImGui::Checkbox(XORSTR("Auto Crouch"), &Settings::Ragebot::AutoCrouch::enabled);
					ImGui::Checkbox(XORSTR("AutoKnife"), &Settings::AutoKnife::enabled);
					if (ImGui::Checkbox(XORSTR("Silent Aim"), &silent))
						UI::UpdateRageWeaponSettings();
					ImGui::Separator();
				}
				// END of other Settings

				// ImGui::ColorButton(XORSTR("MainColor"), Settings::UI::mainColor.ColorVec4, ImGuiColorEditFlags__DataTypeMask, ImVec2(15,15));

				ImGui::Columns(1, nullptr, false);
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
		ImGui::EndColumns();
	}
	ImGui::EndGroup();
}