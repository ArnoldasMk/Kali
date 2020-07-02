#include "legitbottab.h"
#include "ragebottab.h"

#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../Windows/configs.h"
#include "../Windows/colors.h"
#include "../../Hacks/legitbot.h"
#include "triggerbottab.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;
static bool silent = false;
static bool friendly = false;

static bool closestBone = false;
static bool desiredBones[] = {true, true, true, true, true, true, true, // center mass
							  true, true, true, true, true, true, true, // left arm
							  true, true, true, true, true, true, true, // right arm
							  true, true, true, true, true, // left leg
							  true, true, true, true, true  // right leg
							 };
static bool engageLock = false;
static bool engageLockTR = false; // Target Reacquisition
static int engageLockTTR = 700; // Time to Target Reacquisition ( in ms )
static Bone bone = BONE_HEAD;
static ButtonCode_t aimkey = ButtonCode_t::MOUSE_MIDDLE;
static bool aimkeyOnly = false;
static bool smoothEnabled = false;
static float smoothValue = 0.5f;
static SmoothType smoothType = SmoothType::SLOW_END;
static bool smoothSaltEnabled = false;
static float smoothSaltMultiplier = 0.0f;
static bool errorMarginEnabled = false;
static float errorMarginValue = 0.0f;
static bool autoAimEnabled = false;
static bool shootassist = false;
static float LegitautoAimValue = 15.0f;
static bool aimStepEnabled = false;
static float aimStepMin = 25.0f;
static float aimStepMax = 35.0f;
static bool rcsEnabled = false;
static bool rcsAlwaysOn = false;
static float rcsAmountX = 2.0f;
static float rcsAmountY = 2.0f;
static bool autoPistolEnabled = false;
//static bool autoShootEnabled = false;
static bool autoScopeEnabled = false;
static bool noShootEnabled = false;
static bool ignoreJumpEnabled = false;
static bool ignoreEnemyJumpEnabled = false;
static bool smokeCheck = false;
static bool flashCheck = false;
static bool hitchanceEnaled = false;
static float hitchance = 100.f;
static int shotDelay = 123.f;
static int minShotFire = 6;
static bool autoWallEnabled = false;
static float MinDamage = 10.f;
static bool autoAimRealDistance = false;
static bool autoSlow = false;
static bool predEnabled = false;
static bool scopeControlEnabled = false;
static bool TriggerBot = false;

void UI::ReloadWeaponSettings()
{
	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end())
		{
			index = currentWeapon;
		}

	silent = Settings::Legitbot::weapons.at(index).silent;
	friendly = Settings::Legitbot::weapons.at(index).friendly;
	closestBone = Settings::Legitbot::weapons.at(index).closestBone;
	engageLock = Settings::Legitbot::weapons.at(index).engageLock;
	engageLockTR = Settings::Legitbot::weapons.at(index).engageLockTR;
	engageLockTTR = Settings::Legitbot::weapons.at(index).engageLockTTR;
	bone = Settings::Legitbot::weapons.at(index).bone;
	aimkey = Settings::Legitbot::weapons.at(index).aimkey;
	aimkeyOnly = Settings::Legitbot::weapons.at(index).aimkeyOnly;
	smoothEnabled = Settings::Legitbot::weapons.at(index).smoothEnabled;
	smoothValue = Settings::Legitbot::weapons.at(index).smoothAmount;
	smoothType = Settings::Legitbot::weapons.at(index).smoothType;
	smoothSaltEnabled = Settings::Legitbot::weapons.at(index).smoothSaltEnabled;
	smoothSaltMultiplier = Settings::Legitbot::weapons.at(index).smoothSaltMultiplier;
	errorMarginEnabled = Settings::Legitbot::weapons.at(index).errorMarginEnabled;
	errorMarginValue = Settings::Legitbot::weapons.at(index).errorMarginValue;
	autoAimEnabled = Settings::Legitbot::weapons.at(index).autoAimEnabled;
	shootassist = Settings::Legitbot::weapons.at(index).shootassist;
	LegitautoAimValue = Settings::Legitbot::weapons.at(index).LegitautoAimFov;
	aimStepEnabled = Settings::Legitbot::weapons.at(index).aimStepEnabled;
	aimStepMin = Settings::Legitbot::weapons.at(index).aimStepMin;
	aimStepMax = Settings::Legitbot::weapons.at(index).aimStepMax;
	rcsEnabled = Settings::Legitbot::weapons.at(index).rcsEnabled;
	rcsAlwaysOn = Settings::Legitbot::weapons.at(index).rcsAlwaysOn;
	rcsAmountX = Settings::Legitbot::weapons.at(index).rcsAmountX;
	rcsAmountY = Settings::Legitbot::weapons.at(index).rcsAmountY;
	autoPistolEnabled = Settings::Legitbot::weapons.at(index).autoPistolEnabled;
	autoScopeEnabled = Settings::Legitbot::weapons.at(index).autoScopeEnabled;
	noShootEnabled = Settings::Legitbot::weapons.at(index).noShootEnabled;
	ignoreJumpEnabled = Settings::Legitbot::weapons.at(index).ignoreJumpEnabled;
	ignoreEnemyJumpEnabled = Settings::Legitbot::weapons.at(index).ignoreEnemyJumpEnabled;
	smokeCheck = Settings::Legitbot::weapons.at(index).smokeCheck;
	flashCheck = Settings::Legitbot::weapons.at(index).flashCheck;
	hitchanceEnaled = Settings::Legitbot::weapons.at(index).hitchanceEnaled;
	hitchance = Settings::Legitbot::weapons.at(index).hitchance;
	shotDelay = Settings::Legitbot::weapons.at(index).shotDelay;
	minShotFire = Settings::Legitbot::weapons.at(index).minShotFire;
	autoWallEnabled = Settings::Legitbot::weapons.at(index).autoWallEnabled;
	MinDamage = Settings::Legitbot::weapons.at(index).MinDamage;
	autoAimRealDistance = Settings::Legitbot::weapons.at(index).autoAimRealDistance;
	autoSlow = Settings::Legitbot::weapons.at(index).autoSlow;
	predEnabled = Settings::Legitbot::weapons.at(index).predEnabled;
	scopeControlEnabled = Settings::Legitbot::weapons.at(index).scopeControlEnabled;
	TriggerBot = Settings::Legitbot::weapons.at(index).TriggerBot;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		desiredBones[bone] = Settings::Legitbot::weapons.at(index).desiredBones[bone];

	Legitbot::UpdateValues();
}

void UI::UpdateWeaponSettings()
{
	if (Settings::Legitbot::weapons.find(currentWeapon) == Settings::Legitbot::weapons.end() && Settings::Legitbot::enabled)
		{
			Settings::Legitbot::weapons[currentWeapon] = AimbotWeapon_t();
		}

	AimbotWeapon_t settings = {
			.silent = silent,
			.friendly = friendly,
			.closestBone = closestBone,
			.engageLock = engageLock,
			.engageLockTR = engageLockTR,
			.aimkeyOnly = aimkeyOnly,
			.smoothEnabled = smoothEnabled,
			.smoothSaltEnabled = smoothSaltEnabled,
			.errorMarginEnabled = errorMarginEnabled,
			.autoAimEnabled = autoAimEnabled,
			.shootassist = shootassist,
			.aimStepEnabled = aimStepEnabled,
			.rcsEnabled = rcsEnabled,
			.rcsAlwaysOn = rcsAlwaysOn,
			.hitchanceEnaled = hitchanceEnaled,
			.autoPistolEnabled = autoPistolEnabled,
			.autoScopeEnabled = autoScopeEnabled,
			.noShootEnabled = noShootEnabled,
			.ignoreJumpEnabled = ignoreJumpEnabled,
			.ignoreEnemyJumpEnabled = ignoreEnemyJumpEnabled,
			.smokeCheck = smokeCheck,
			.flashCheck = flashCheck,
			.autoWallEnabled = autoWallEnabled,
			.autoAimRealDistance = autoAimRealDistance,
			.autoSlow = autoSlow,
			.predEnabled = predEnabled,
			.scopeControlEnabled = scopeControlEnabled,
			.TriggerBot = TriggerBot,
			.engageLockTTR = engageLockTTR,
			.bone = bone,
			.smoothType = smoothType,
			.aimkey = aimkey,
			.smoothAmount = smoothValue,
			.smoothSaltMultiplier = smoothSaltMultiplier,
			.errorMarginValue = errorMarginValue,
			.LegitautoAimFov = LegitautoAimValue,
			.aimStepMin = aimStepMin,
			.aimStepMax = aimStepMax,
			.rcsAmountX = rcsAmountX,
			.rcsAmountY = rcsAmountY,
			.MinDamage = MinDamage,
			.hitchance = hitchance,
			.shotDelay = shotDelay,
			.minShotFire = minShotFire,
	};



	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		settings.desiredBones[bone] = desiredBones[bone];

	Settings::Legitbot::weapons.at(currentWeapon) = settings;

	if (Settings::Legitbot::weapons.at(currentWeapon) == Settings::Legitbot::weapons.at(ItemDefinitionIndex::INVALID) &&
		currentWeapon != ItemDefinitionIndex::INVALID)
	{
		Settings::Legitbot::weapons.erase(currentWeapon);
		UI::ReloadWeaponSettings();
		return;
	}
	Legitbot::UpdateValues();
}

void Legitbot::RenderTab()
{
	const char* targets[] = { "PELVIS", "", "", "HIP", "LOWER SPINE", "MIDDLE SPINE", "UPPER SPINE", "NECK", "HEAD" };
	const char* smoothTypes[] = { "Slow Near End", "Constant Speed", "Fast Near End" };
	static char filterWeapons[32];

	

	ImGui::Columns(3, nullptr, false);
	{
		ImGui::SetColumnOffset(1, 200);
		ImGui::PushItemWidth(-10);

		if (ImGui::Checkbox(XORSTR("Enabled"), &Settings::Legitbot::enabled))
		{	
			Settings::Ragebot::enabled = false;
			UI::UpdateWeaponSettings();
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
			bool isChanged = Settings::Legitbot::weapons.find(it.first) != Settings::Legitbot::weapons.end();
			if (!isDefault && isChanged)
				changeIndicator = '*';
			formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str() : Util::Items::GetItemDisplayName(it.first));
			if (ImGui::Selectable(formattedName.c_str(), item_selected))
			{
				currentWeapon = it.first;
				UI::ReloadWeaponSettings();
			}
			ImGui::PopID();
		}
		ImGui::ListBoxFooter();
	}
	
	ImGui::NextColumn();
	{
		for (int i = 0; i < 6; i++)
			ImGui::Spacing();

		ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 75);
		ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), false);
		{
			ImGui::Columns(1, nullptr, false);
			{
				ImGui::PushItemWidth(-1);
				
				ImGui::TextWrapped(XORSTR("Priority Hitbox"));
				ImGui::SameLine(0.0f, -1.0f);	
				if (ImGui::Combo(XORSTR("##AIMTARGET"), (int*)&bone, targets, IM_ARRAYSIZE(targets)))
					UI::UpdateWeaponSettings();

				ImGui::Columns(1);
					if(ImGui::Button(XORSTR("Select Bones"), ImVec2(-1, 0)))
						ImGui::OpenPopup(XORSTR("optionBones"));
					ImGui::SetNextWindowSize(ImVec2((ImGui::GetWindowWidth()/1.25f),ImGui::GetWindowHeight()), ImGuiSetCond_Always);
					if( ImGui::BeginPopup(XORSTR("optionBones")) )
					{
						ImGui::PushItemWidth(-1);
						ImGui::Text(XORSTR("Center Mass"));
						if( ImGui::Checkbox(XORSTR("Head"), &desiredBones[BONE_HEAD]) )
							UI::UpdateWeaponSettings();
						if( ImGui::Checkbox(XORSTR("Neck"), &desiredBones[BONE_NECK]) )
							UI::UpdateWeaponSettings();
						if( ImGui::Checkbox(XORSTR("Upper Spine"), &desiredBones[BONE_UPPER_SPINAL_COLUMN]) )
							UI::UpdateWeaponSettings();
						if( ImGui::Checkbox(XORSTR("Middle Spine"), &desiredBones[BONE_MIDDLE_SPINAL_COLUMN]) )
							UI::UpdateWeaponSettings();
						if( ImGui::Checkbox(XORSTR("Lower Spine"), &desiredBones[BONE_LOWER_SPINAL_COLUMN]) )
							UI::UpdateWeaponSettings();
						if( ImGui::Checkbox(XORSTR("Pelvis"), &desiredBones[BONE_PELVIS]) )
							UI::UpdateWeaponSettings();
						if( ImGui::Checkbox(XORSTR("Hip"), &desiredBones[BONE_HIP]) )
							UI::UpdateWeaponSettings();
						ImGui::Separator();

						ImGui::Columns(2, nullptr, false);
						{
							ImGui::Text(XORSTR("Player's Right Arm"));
							if( ImGui::Checkbox(XORSTR("Collarbone"), &desiredBones[BONE_RIGHT_COLLARBONE]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Shoulder"), &desiredBones[BONE_RIGHT_SHOULDER]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Armpit"), &desiredBones[BONE_RIGHT_ARMPIT]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Bicep"), &desiredBones[BONE_RIGHT_BICEP]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Elbow"), &desiredBones[BONE_RIGHT_ELBOW]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Forearm"), &desiredBones[BONE_RIGHT_FOREARM]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Wrist"), &desiredBones[BONE_RIGHT_WRIST]) )
								UI::UpdateWeaponSettings();
							ImGui::Text(XORSTR("Player's Right Leg"));
							if( ImGui::Checkbox(XORSTR("Buttcheek"), &desiredBones[BONE_RIGHT_BUTTCHEEK]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Thigh"), &desiredBones[BONE_RIGHT_THIGH]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Knee"), &desiredBones[BONE_RIGHT_KNEE]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Ankle"), &desiredBones[BONE_RIGHT_ANKLE]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Sole"), &desiredBones[BONE_RIGHT_SOLE]) )
								UI::UpdateWeaponSettings();
						}
						ImGui::NextColumn();
						{   // these spaces are here in the strings because checkboxes can't have duplicate titles.
							ImGui::Text(XORSTR("Player's Left Arm"));
							if( ImGui::Checkbox(XORSTR("Collarbone "), &desiredBones[BONE_LEFT_COLLARBONE]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Shoulder "), &desiredBones[BONE_LEFT_SHOULDER]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Armpit "), &desiredBones[BONE_LEFT_ARMPIT]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Bicep "), &desiredBones[BONE_LEFT_BICEP]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Elbow "), &desiredBones[BONE_LEFT_ELBOW]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Forearm "), &desiredBones[BONE_LEFT_FOREARM]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Wrist "), &desiredBones[BONE_LEFT_WRIST]) )
								UI::UpdateWeaponSettings();

							ImGui::Text(XORSTR("Player's Left Leg"));
							if( ImGui::Checkbox(XORSTR("Buttcheek "), &desiredBones[BONE_LEFT_BUTTCHEEK]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Thigh "), &desiredBones[BONE_LEFT_THIGH]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Knee "), &desiredBones[BONE_LEFT_KNEE]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Ankle "), &desiredBones[BONE_LEFT_ANKLE]) )
								UI::UpdateWeaponSettings();
							if( ImGui::Checkbox(XORSTR("Sole "), &desiredBones[BONE_LEFT_SOLE]) )
								UI::UpdateWeaponSettings();
						}
						ImGui::PopItemWidth();
						ImGui::EndPopup();
					}
			
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1, nullptr, true);
			// Spcacing Between Elements
			ImGui::Spacing(); ImGui::Spacing(); 

			//Aim Options Assist Options
			if (ImGui::Checkbox(XORSTR("Auto Aim"), &autoAimEnabled))
				UI::UpdateWeaponSettings();
			
			if (!autoAimEnabled)
				shootassist = false;

			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			if (ImGui::SliderFloat(XORSTR("##Fov"), &LegitautoAimValue, 0, 15, XORSTR("View Angle : %0.3f")))
				UI::UpdateWeaponSettings();
			ImGui::PopItemWidth();
			//END AIM ASSIST FEATURES

			ImGui::Columns(1);
			ImGui::Spacing(); ImGui::Spacing();

			ImGui::Columns(2, nullptr, false);
			{
				if (ImGui::Checkbox(XORSTR("Recoil Control"), &rcsEnabled))
					UI::UpdateWeaponSettings();
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::PopItemWidth();
				if (ImGui::Button(XORSTR("RCS Settings"), ImVec2(-1, 0)))
					ImGui::OpenPopup(XORSTR("optionRCSAmount"));
				ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_Always);
				if (ImGui::BeginPopup(XORSTR("optionRCSAmount")))
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::Checkbox(XORSTR("RCS Always on"), &rcsAlwaysOn))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##RCSX"), &rcsAmountX, 0, 2, XORSTR("Pitch: %0.3f")))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##RCSY"), &rcsAmountY, 0, 2, XORSTR("Yaw: %0.3f")))
						UI::UpdateWeaponSettings();
					ImGui::PopItemWidth();

					ImGui::EndPopup();
				}
			}
			ImGui::EndColumns();
			ImGui::Spacing(); ImGui::Spacing();

			ImGui::Columns(2, nullptr, false);
			{
				if (ImGui::Checkbox(XORSTR("Smoothing"), &smoothEnabled))
					UI::UpdateWeaponSettings();
				if (ImGui::Checkbox(XORSTR("Smooth Salting"), &smoothSaltEnabled))
					UI::UpdateWeaponSettings();
				if (ImGui::Checkbox(XORSTR("Error Margin"), &errorMarginEnabled))
					UI::UpdateWeaponSettings();
				
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat(XORSTR("##SMOOTH"), &smoothValue, 0, 1))
					UI::UpdateWeaponSettings();
				if (ImGui::SliderFloat(XORSTR("##SALT"), &smoothSaltMultiplier, 0, smoothValue))
					UI::UpdateWeaponSettings();
				if (ImGui::SliderFloat(XORSTR("##ERROR"), &errorMarginValue, 0, 2))
					UI::UpdateWeaponSettings();
				ImGui::PopItemWidth();
			}
			ImGui::EndColumns();
			
			// Smooth Type Selection
			ImGui::Spacing(); ImGui::Spacing();
			ImGui::Columns(1, nullptr, false);
			{
				ImGui::TextWrapped(XORSTR("Smooth Type"));
				ImGui::PushItemWidth(-1);
				if (ImGui::Combo(XORSTR("##SMOOTHTYPE"), (int*)& smoothType, smoothTypes, IM_ARRAYSIZE(smoothTypes)))
					UI::UpdateWeaponSettings();
				ImGui::PopItemWidth();
			}
			// END of Smooth type Selection

			if ( ImGui::Checkbox(XORSTR("Trigger Bot"), &TriggerBot) )
				UI::UpdateWeaponSettings();
			ImGui::Checkbox(XORSTR("Magnet"), &Settings::Triggerbot::Magnet::enabled);
			/*
			ImGui::Text(XORSTR("Keybind"));
				ImGui::Separator();
				ImGui::Columns(2, nullptr, true);
				{
					ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
					ImGui::Text(XORSTR("Trigger Key"));
				}
				ImGui::NextColumn();
				{
					UI::KeyBindButton(&Settings::Triggerbot::key);
				}
				*/
			ImGui::Spacing();
			ImGui::Checkbox(XORSTR("Randome Delay"), &Settings::Triggerbot::RandomDelay::enabled);

			if (Settings::Triggerbot::RandomDelay::enabled)
			{	
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				if (ImGui::BeginCombo(XORSTR("##RandomeDelay"), XORSTR("Random Min Max")))
				{
				
					if( Settings::Triggerbot::RandomDelay::lastRoll != 0 )
						ImGui::Text(XORSTR("Last delay: %dms"), Settings::Triggerbot::RandomDelay::lastRoll);
					ImGui::Text(XORSTR("Minimum ms"));
					ImGui::SliderInt(XORSTR("##TRIGGERRANDOMLOW"), &Settings::Triggerbot::RandomDelay::lowBound, 5, 220);
					if( Settings::Triggerbot::RandomDelay::lowBound >= Settings::Triggerbot::RandomDelay::highBound )
						Settings::Triggerbot::RandomDelay::highBound = Settings::Triggerbot::RandomDelay::lowBound + 1;
					ImGui::Text(XORSTR("Maximum ms"));
					ImGui::SliderInt(XORSTR("##TRIGGERRANDOMHIGH"), &Settings::Triggerbot::RandomDelay::highBound, (Settings::Triggerbot::RandomDelay::lowBound+1), 225);
			
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();
			}

			ImGui::Spacing();
 			ImGui::Checkbox(XORSTR("Auto Knife"), &Settings::AutoKnife::enabled);
			ImGui::SameLine();
 			ImGui::PushItemWidth(-1);
			if ( ImGui::BeginCombo(XORSTR("##FilterAutoKnife"),XORSTR("Filter Auto Knife")) )
			{	
				ImGui::Selectable(XORSTR("Enemies Auto Knife"), &Settings::AutoKnife::Filters::enemies, ImGuiSelectableFlags_DontClosePopups);
 				ImGui::Selectable(XORSTR("Allies Auto Knife"), &Settings::AutoKnife::Filters::allies, ImGuiSelectableFlags_DontClosePopups);
				
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			if ( ImGui::BeginCombo(XORSTR("##Filter"),XORSTR("Filter")) )
			{
				ImGui::Selectable(XORSTR("Enemies"), &Settings::Triggerbot::Filters::enemies, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Walls"), &Settings::Triggerbot::Filters::walls, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Head"), &Settings::Triggerbot::Filters::head, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Chest"), &Settings::Triggerbot::Filters::chest, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Legs"), &Settings::Triggerbot::Filters::legs, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Allies"), &Settings::Triggerbot::Filters::allies, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Smoke check"), &Settings::Triggerbot::Filters::smokeCheck, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Flash check"), &Settings::Triggerbot::Filters::flashCheck, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Stomach"), &Settings::Triggerbot::Filters::stomach, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Arms"), &Settings::Triggerbot::Filters::arms, ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}
		}
		ImGui::EndChild();
	}
	
	ImGui::NextColumn();
	{
		for (int i = 0; i < 6; i++)
			ImGui::Spacing();

		ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), false);
		{

			ImGui::Columns(1, nullptr, false);
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat(XORSTR("##AUTOWALLDMG"), &MinDamage, 0, 100, XORSTR("Min Damage: %.0f")))
					UI::UpdateWeaponSettings();
				ImGui::PopItemWidth();
			}
			ImGui::Spacing(); ImGui::Spacing();
			ImGui::Columns(2, nullptr, false);
			{
				if (ImGui::Checkbox(XORSTR("Aimkey Only"), &aimkeyOnly))
					UI::UpdateWeaponSettings();
			}
			ImGui::NextColumn();
			{
				if (aimkeyOnly)
				{
					UI::KeyBindButton(&aimkey);
					ImGui::Spacing();	ImGui::Spacing();
				}	
			}
			ImGui::EndColumns();

			ImGui::Columns(1);

			// AimStep Settings
			ImGui::Columns(1, nullptr, false);
			{
				if (ImGui::Checkbox(XORSTR("Aim Step"), &aimStepEnabled))
					UI::UpdateWeaponSettings();
				
				if ( aimStepEnabled )
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::SliderFloat(XORSTR("##STEPMIN"), &aimStepMin, 5, 35, XORSTR("MIN : %0.0f")))
						UI::UpdateWeaponSettings();
				
					ImGui::SameLine();
					if (ImGui::SliderFloat(XORSTR("##STEPMAX"), &aimStepMax, (aimStepMin) + 1.0f, 35, XORSTR("MAX : %0.0f")))
						UI::UpdateWeaponSettings();
					ImGui::PopItemWidth();

					ImGui::Spacing(); ImGui::Spacing();
				}
				
			}

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
							UI::UpdateWeaponSettings();
						break;
					default:
						break;
				}

				if (ImGui::Checkbox(XORSTR("Silent Aim"), &silent))
					UI::UpdateWeaponSettings();
				if (ImGui::Checkbox(XORSTR("Prediction"), &predEnabled))
					UI::UpdateWeaponSettings();

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
							UI::UpdateWeaponSettings();
				}

				if (ImGui::Checkbox(XORSTR("Ignore Jump (Self)"), &ignoreJumpEnabled))
					UI::UpdateWeaponSettings();
				if (ImGui::Checkbox(XORSTR("Ignore Jump (Enemies)"), &ignoreEnemyJumpEnabled))
					UI::UpdateWeaponSettings();
			}

			if (ImGui::Checkbox(XORSTR("Auto Slow"), &autoSlow))
				UI::UpdateWeaponSettings();
			
			ImGui::Checkbox(XORSTR("Back Track"), &Settings::Ragebot::backTrack::enabled);
			
			ImGui::Columns(1, nullptr, false);
			ImGui::Spacing(); ImGui::Spacing();

			if (currentWeapon > ItemDefinitionIndex::INVALID && Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end())
			{
				if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0)))
				{
					Settings::Legitbot::weapons.erase(currentWeapon);
					UI::ReloadWeaponSettings();
				}
			}
			ImGui::EndChild();
			/*
			ImGui::BeginChild(XORSTR("TRIG1"), ImVec2(0, 0), true);
			{
				*/
				
/*
				ImGui::EndChild();
			}
			*/
		}
	}
	
	ImGui::EndColumns();
	
}


