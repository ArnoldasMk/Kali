#include "legitbottab.h"
#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;
static bool enabled = false;
static bool silent = false;
static bool friendly = false;

static bool closestBone = true;
inline bool desiredBones[] = {
    true, true, true, true, true, true, true,	   // center mass
    false, false, false, false, false, false, false, // left arm
    false, false, false, false, false, false, false, // right arm
    false, false, false, false, false,			   // left leg
    false, false, false, false, false			   // right leg
};
static bool closestHitbox = false;
static bool engageLock = false;
static bool engageLockTR = false; // Target Reacquisition
static int engageLockTTR = 700;   // Time to Target Reacquisition ( in ms )
static Bone bone = CONST_BONE_HEAD;
static ButtonCode_t aimkey = ButtonCode_t::MOUSE_MIDDLE;
static bool aimkeyOnly = false;
static bool smoothEnabled = false;
static float smoothValue = 0.5f;
static bool courseRandomizationEnabled = false;
static bool doAimAfterXShotsEnabled = false;
static float courseRandomizationValue = 2.0f;
static float doAimAfterXShotsValue = 0.0f;
static SmoothType smoothType = SmoothType::SLOW_END;
static bool smoothSaltEnabled = false;
static float smoothSaltMultiplier = 0.0f;
static bool errorMarginEnabled = false;
static float errorMarginValue = 0.0f;
static bool curveEnabled = false;
static float curveAmount = 0.5f;
static bool autoAimEnabled = false;
static float autoAimValue = 180.0f;
static bool aimStepEnabled = false;
static float aimStepMin = 25.0f;
static float aimStepMax = 35.0f;
static bool rcsEnabled = false;
static bool rcsAlwaysOn = false;
static float rcsAmountX = 2.0f;
static float rcsAmountY = 2.0f;
static bool autoPistolEnabled = false;
static bool autoShootEnabled = false;
static bool autoScopeEnabled = false;
static bool noShootEnabled = false;
static bool ignoreJumpEnabled = false;
static bool ignoreEnemyJumpEnabled = false;
static bool smokeCheck = false;
static bool flashCheck = false;
static bool spreadLimitEnabled = false;
static float spreadLimit = 0.1f;
static bool autoAimRealDistance = false;
static bool autoSlow = false;
static bool predEnabled = false;
static bool scopeControlEnabled = false;
static bool hitchanceEnabled = false;
static float hitchanceValue = 100.f;
static bool triggerBotEnabled = false;
static bool triggerHitchanceEnabled = false;
static float triggerHitchanceValue = 100.f;

void UI::ReloadWeaponSettings()
{
	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end())
		index = currentWeapon;

	silent = Settings::Legitbot::weapons.at(index).silent;
	friendly = Settings::Legitbot::weapons.at(index).friendly;
	engageLock = Settings::Legitbot::weapons.at(index).engageLock;
	engageLockTR = Settings::Legitbot::weapons.at(index).engageLockTR;
	engageLockTTR = Settings::Legitbot::weapons.at(index).engageLockTTR;
	bone = Settings::Legitbot::weapons.at(index).bone;
	aimkey = Settings::Legitbot::weapons.at(index).aimkey;
	aimkeyOnly = Settings::Legitbot::weapons.at(index).aimkeyOnly;
	smoothEnabled = Settings::Legitbot::weapons.at(index).smoothEnabled;
	courseRandomizationEnabled = Settings::Legitbot::weapons.at(index).courseRandomizationEnabled;
	doAimAfterXShotsEnabled = Settings::Legitbot::weapons.at(index).doAimAfterXShotsEnabled;
	smoothValue = Settings::Legitbot::weapons.at(index).smoothAmount;
	courseRandomizationValue = Settings::Legitbot::weapons.at(index).courseRandomizationAmount;
	doAimAfterXShotsValue = Settings::Legitbot::weapons.at(index).doAimAfterXShotsAmount;
	smoothType = Settings::Legitbot::weapons.at(index).smoothType;
	smoothSaltEnabled = Settings::Legitbot::weapons.at(index).smoothSaltEnabled;
	smoothSaltMultiplier = Settings::Legitbot::weapons.at(index).smoothSaltMultiplier;
	errorMarginEnabled = Settings::Legitbot::weapons.at(index).errorMarginEnabled;
	errorMarginValue = Settings::Legitbot::weapons.at(index).errorMarginValue;
	curveEnabled = Settings::Legitbot::weapons.at(index).curveAmount;
	curveAmount = Settings::Legitbot::weapons.at(index).curveAmount;
	autoAimEnabled = Settings::Legitbot::weapons.at(index).autoAimEnabled;
	autoAimValue = Settings::Legitbot::weapons.at(index).LegitautoAimFov;
	aimStepEnabled = Settings::Legitbot::weapons.at(index).aimStepEnabled;
	aimStepMin = Settings::Legitbot::weapons.at(index).aimStepMin;
	aimStepMax = Settings::Legitbot::weapons.at(index).aimStepMax;
	rcsEnabled = Settings::Legitbot::weapons.at(index).rcsEnabled;
	rcsAlwaysOn = Settings::Legitbot::weapons.at(index).rcsAlwaysOn;
	rcsAmountX = Settings::Legitbot::weapons.at(index).rcsAmountX;
	rcsAmountY = Settings::Legitbot::weapons.at(index).rcsAmountY;
	autoPistolEnabled = Settings::Legitbot::weapons.at(index).autoPistolEnabled;
	autoShootEnabled = Settings::Legitbot::weapons.at(index).autoShootEnabled;
	autoScopeEnabled = Settings::Legitbot::weapons.at(index).autoScopeEnabled;
	noShootEnabled = Settings::Legitbot::weapons.at(index).noShootEnabled;
	ignoreJumpEnabled = Settings::Legitbot::weapons.at(index).ignoreJumpEnabled;
	ignoreEnemyJumpEnabled = Settings::Legitbot::weapons.at(index).ignoreEnemyJumpEnabled;
	hitchanceEnabled = Settings::Legitbot::weapons.at(index).hitchanceEnabled;
	hitchanceValue = Settings::Legitbot::weapons.at(index).hitchanceValue;
	smokeCheck = Settings::Legitbot::weapons.at(index).smokeCheck;
	flashCheck = Settings::Legitbot::weapons.at(index).flashCheck;
	spreadLimitEnabled = Settings::Legitbot::weapons.at(index).spreadLimitEnabled;
	spreadLimit = Settings::Legitbot::weapons.at(index).spreadLimit;
	autoAimRealDistance = Settings::Legitbot::weapons.at(index).autoAimRealDistance;
	autoSlow = Settings::Legitbot::weapons.at(index).autoSlow;
	predEnabled = Settings::Legitbot::weapons.at(index).predEnabled;
	scopeControlEnabled = Settings::Legitbot::weapons.at(index).scopeControlEnabled;
	triggerBotEnabled = Settings::Legitbot::weapons.at(index).triggerBotEnabled;
	triggerHitchanceValue = Settings::Legitbot::weapons.at(index).triggerHitchanceValue;
	triggerHitchanceEnabled = Settings::Legitbot::weapons.at(index).triggerHitchanceEnabled;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		desiredBones[bone] = Settings::Legitbot::weapons.at(index).desiredBones[bone];
}

void UI::UpdateWeaponSettings()
{
	if (Settings::Legitbot::weapons.find(currentWeapon) == Settings::Legitbot::weapons.end())
		Settings::Legitbot::weapons[currentWeapon] = LegitWeapon_t();

	LegitWeapon_t settings = {
	    .silent = silent,
	    .friendly = friendly,
	    .engageLock = engageLock,
	    .engageLockTR = engageLockTR,
	    .aimkeyOnly = aimkeyOnly,
	    .smoothEnabled = smoothEnabled,
	    .courseRandomizationEnabled = courseRandomizationEnabled,
	    .doAimAfterXShotsEnabled = doAimAfterXShotsEnabled,
	    .smoothSaltEnabled = smoothSaltEnabled,
	    .errorMarginEnabled = errorMarginEnabled,
	    .autoAimEnabled = autoAimEnabled,
	    .aimStepEnabled = aimStepEnabled,
	    .rcsEnabled = rcsEnabled,
	    .rcsAlwaysOn = rcsAlwaysOn,
	    .spreadLimitEnabled = spreadLimitEnabled,
	    .hitchanceEnabled = hitchanceEnabled,
	    .autoPistolEnabled = autoPistolEnabled,
	    .autoShootEnabled = autoShootEnabled,
	    .autoScopeEnabled = autoScopeEnabled,
	    .noShootEnabled = noShootEnabled,
	    .ignoreJumpEnabled = ignoreJumpEnabled,
	    .ignoreEnemyJumpEnabled = ignoreEnemyJumpEnabled,
	    .smokeCheck = smokeCheck,
	    .flashCheck = flashCheck,
	    .autoAimRealDistance = autoAimRealDistance,
	    .autoSlow = autoSlow,
	    .predEnabled = predEnabled,
	    .scopeControlEnabled = scopeControlEnabled,
	    .triggerBotEnabled = triggerBotEnabled,
	    .triggerHitchanceEnabled = triggerHitchanceEnabled,

	    .engageLockTTR = engageLockTTR,
	    .bone = bone,
	    .smoothType = smoothType,
	    .aimkey = aimkey,
	    .smoothAmount = smoothValue,
	    .courseRandomizationAmount = courseRandomizationValue,
	    .doAimAfterXShotsAmount = doAimAfterXShotsValue,
	    .smoothSaltMultiplier = smoothSaltMultiplier,
	    .errorMarginValue = errorMarginValue,
	    .LegitautoAimFov = autoAimValue,
	    .aimStepMin = aimStepMin,
	    .aimStepMax = aimStepMax,
	    .rcsAmountX = rcsAmountX,
	    .rcsAmountY = rcsAmountY,
	    .spreadLimit = spreadLimit,
	    .minDamagevalue = 10.0f,
	    .hitchanceValue = hitchanceValue,
	    .triggerHitchanceValue = triggerHitchanceValue,

	};
	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		settings.desiredBones[bone] = desiredBones[bone];
	Settings::Legitbot::weapons.at(currentWeapon) = settings;

	if (Settings::Legitbot::weapons.at(currentWeapon) == Settings::Legitbot::weapons.at(ItemDefinitionIndex::INVALID) &&
	    currentWeapon != ItemDefinitionIndex::INVALID)
	{
		Settings::Legitbot::weapons.erase(currentWeapon);
		UI::ReloadWeaponSettings();
	}
}

void Legitbot::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	const char *targets[] = {"PELVIS", "HIP", "LOWER SPINE", "MIDDLE SPINE", "UPPER SPINE", "NECK", "HEAD"};
	const char *smoothTypes[] = {"Slow Near End", "Constant Speed", "Fast Near End"};
	static char filterWeapons[32];
	draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 960 - 15, pos.y + 95), ImColor(0, 0, 0, 25), 10);
	ImGui::SetCursorPos(ImVec2(15, 70));
	ImGui::BeginGroup();
	{
		if (ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Legitbot::enabled))
		{
			Settings::Ragebot::enabled = false;
			UI::UpdateWeaponSettings();
		}
	}
	ImGui::EndGroup();
	ImGui::BeginGroup();
	{
		ImGui::Columns(3, nullptr, false);
		{
			ImGui::SetColumnOffset(1, 200);
			ImGui::PushItemWidth(-1);
			ImGui::InputText(XORSTR("##FILTERWEAPONS"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
			ImGui::PopItemWidth();
			ImGui::ListBoxHeader(XORSTR("##GUNS"), ImVec2(-1, 706));
			for (auto it : ItemDefinitionIndexMap)
			{
				bool isDefault = (int)it.first < 0;
				if (!isDefault && !Util::Contains(Util::ToLower(std::string(filterWeapons)), Util::ToLower(Util::Items::GetItemDisplayName(it.first).c_str())))
					continue;

				if (Util::Items::IsKnife(it.first) || Util::Items::IsGlove(it.first) || Util::Items::IsUtility(it.first))
					continue;

				const bool item_selected = ((int)it.first == (int)currentWeapon);
				ImGui::PushID((int)it.first);
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
			ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 150);
			ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 736), true);
			{
				ImGui::Text(XORSTR("Accuracy"));
				ImGui::Columns(2, nullptr, false);
				{
					if (ImGui::Checkbox(XORSTR("Auto Aim"), &autoAimEnabled))
						UI::UpdateWeaponSettings();
					if (ImGui::Button(XORSTR("RCS"), ImVec2(-1, 0)))
						ImGui::OpenPopup(XORSTR("LegitRCS"));
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::SliderFloat(XORSTR("##AA"), &autoAimValue, 0, 180, XORSTR("FOV: %0.f")))
						UI::UpdateWeaponSettings();
					if (ImGui::Button(XORSTR("Target"), ImVec2(-1, 0)))
						ImGui::OpenPopup(XORSTR("LegitTargets"));
					ImGui::PopItemWidth();
				}
				ImGui::EndColumns();

				ImGui::SetNextWindowSize(ImVec2(325, 60), ImGuiCond_Always);
				if (ImGui::BeginPopup(XORSTR("LegitRCS"))) {
					ImGui::Columns(2, nullptr, false);
					{
						if (ImGui::Checkbox(XORSTR("Enabled"), &rcsEnabled))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Always on"), &rcsAlwaysOn))
							UI::UpdateWeaponSettings();
					}
					ImGui::NextColumn();
					{
						ImGui::PushItemWidth(-1);
						if (ImGui::SliderFloat(XORSTR("##RCSX"), &rcsAmountX, 0, 2, XORSTR("Pitch: %0.3f")))
							UI::UpdateWeaponSettings();
						if (ImGui::SliderFloat(XORSTR("##RCSY"), &rcsAmountY, 0, 2, XORSTR("Yaw: %0.3f")))
							UI::UpdateWeaponSettings();
						ImGui::PopItemWidth();
					}
					ImGui::Columns(1);
					ImGui::EndPopup();
				}	

				ImGui::SetNextWindowSize(ImVec2(425, 325), ImGuiCond_Always);
				if (ImGui::BeginPopup(XORSTR("LegitTargets"))) {
					ImGui::Columns(3, nullptr, false);
					{
						ImGui::PushItemWidth(-1);
						ImGui::Text(XORSTR("Player's Right Arm"));
						if (ImGui::Checkbox(XORSTR("Collarbone"), &desiredBones[BONE_RIGHT_COLLARBONE]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Shoulder"), &desiredBones[BONE_RIGHT_SHOULDER]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Armpit"), &desiredBones[BONE_RIGHT_ARMPIT]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Bicep"), &desiredBones[BONE_RIGHT_BICEP]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Elbow"), &desiredBones[BONE_RIGHT_ELBOW]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Forearm"), &desiredBones[BONE_RIGHT_FOREARM]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Wrist"), &desiredBones[BONE_RIGHT_WRIST]))
							UI::UpdateWeaponSettings();
						ImGui::Text(XORSTR("Player's Right Leg"));
						if (ImGui::Checkbox(XORSTR("Buttcheek"), &desiredBones[BONE_RIGHT_BUTTCHEEK]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Thigh"), &desiredBones[BONE_RIGHT_THIGH]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Knee"), &desiredBones[BONE_RIGHT_KNEE]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Ankle"), &desiredBones[BONE_RIGHT_ANKLE]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Sole"), &desiredBones[BONE_RIGHT_SOLE]))
							UI::UpdateWeaponSettings();
						ImGui::PopItemWidth();
					}
					ImGui::NextColumn();
					{
						ImGui::PushItemWidth(-1);
						ImGui::Text(XORSTR("Center Mass"));
						if (ImGui::Checkbox(XORSTR("Head"), &desiredBones[BONE_HEAD]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Neck"), &desiredBones[BONE_NECK]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Upper Spine"), &desiredBones[BONE_UPPER_SPINAL_COLUMN]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Middle Spine"), &desiredBones[BONE_MIDDLE_SPINAL_COLUMN]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Lower Spine"), &desiredBones[BONE_LOWER_SPINAL_COLUMN]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Pelvis"), &desiredBones[BONE_PELVIS]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Hip"), &desiredBones[BONE_HIP]))
							UI::UpdateWeaponSettings();
						ImGui::PopItemWidth();
					}
					ImGui::NextColumn();
					{ // these spaces are here in the strings because checkboxes can't have duplicate titles.
						ImGui::PushItemWidth(-1);
						ImGui::Text(XORSTR("Player's Left Arm"));
						if (ImGui::Checkbox(XORSTR("Collarbone "), &desiredBones[BONE_LEFT_COLLARBONE]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Shoulder "), &desiredBones[BONE_LEFT_SHOULDER]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Armpit "), &desiredBones[BONE_LEFT_ARMPIT]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Bicep "), &desiredBones[BONE_LEFT_BICEP]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Elbow "), &desiredBones[BONE_LEFT_ELBOW]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Forearm "), &desiredBones[BONE_LEFT_FOREARM]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Wrist "), &desiredBones[BONE_LEFT_WRIST]))
							UI::UpdateWeaponSettings();

						ImGui::Text(XORSTR("Player's Left Leg"));
						if (ImGui::Checkbox(XORSTR("Buttcheek "), &desiredBones[BONE_LEFT_BUTTCHEEK]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Thigh "), &desiredBones[BONE_LEFT_THIGH]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Knee "), &desiredBones[BONE_LEFT_KNEE]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Ankle "), &desiredBones[BONE_LEFT_ANKLE]))
							UI::UpdateWeaponSettings();
						if (ImGui::Checkbox(XORSTR("Sole "), &desiredBones[BONE_LEFT_SOLE]))
							UI::UpdateWeaponSettings();
						ImGui::PopItemWidth();
					}
					ImGui::EndColumns();
					ImGui::EndPopup();
				}
				ImGui::Separator();
				ImGui::Text(XORSTR("Humanizing"));
				ImGui::Columns(2, nullptr, false);
				{
					if (ImGui::Checkbox(XORSTR("Curving"), &curveEnabled))
						UI::UpdateWeaponSettings();
					SetTooltip(XORSTR("Curve makes aim not go in a straight line. Used with smoothing"));
					if (ImGui::Checkbox(XORSTR("Smoothing"), &smoothEnabled))
						UI::UpdateWeaponSettings();
					SetTooltip(XORSTR("Smoothing reduces the aimbot \"snap\". 0 for full snap. 1 for full smoothing"));
					if (ImGui::Checkbox(XORSTR("Smooth Salting"), &smoothSaltEnabled))
						UI::UpdateWeaponSettings();
					SetTooltip(XORSTR("Breaks the smoothing into smaller steps, high smooth + low salt is slightly stuttery"));
					if (ImGui::Checkbox(XORSTR("Error Margin"), &errorMarginEnabled))
						UI::UpdateWeaponSettings();
					SetTooltip(XORSTR("Adds a margin of error to the aim, it will be obvious what it does when using it"));
					if (ImGui::Checkbox(XORSTR("Advanced Error"), &courseRandomizationEnabled))
						UI::UpdateWeaponSettings();
					SetTooltip(XORSTR("Course randomization"));
					if (ImGui::Checkbox(XORSTR("Aim After Shot #"), &doAimAfterXShotsEnabled))
						UI::UpdateWeaponSettings();
					SetTooltip(XORSTR("Aim after X shots"));
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::SliderFloat("##CURVE", &curveAmount, 0, 2))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##SMOOTH"), &smoothValue, 0, 1))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##SALT"), &smoothSaltMultiplier, 0, smoothValue))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##ERROR"), &errorMarginValue, 0, 2))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##COURSERANDOMIZATION"), &courseRandomizationValue, 1, 6))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##DOAIMAFTERXSHOTS"), &doAimAfterXShotsValue, 0, 30))
						UI::UpdateWeaponSettings();
					ImGui::PopItemWidth();
				}
				ImGui::EndColumns();				
				ImGui::PushItemWidth(-1);
				if (ImGui::Combo(XORSTR("##SMOOTHTYPE"), (int*)&smoothType, smoothTypes, IM_ARRAYSIZE(smoothTypes)))
					UI::UpdateWeaponSettings();
				ImGui::PopItemWidth();
				ImGui::Separator();
				ImGui::Text(XORSTR("AimStep"));
				ImGui::Columns(2, nullptr, false);
				{
					if (ImGui::Checkbox(XORSTR("Enabled"), &aimStepEnabled))
						UI::UpdateWeaponSettings();
					SetTooltip(XORSTR("Casual/DM only"));
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::SliderFloat(XORSTR("##STEPMIN"), &aimStepMin, 5, 35, XORSTR("AimStep min: %0.f")))
						UI::UpdateWeaponSettings();
					if (ImGui::SliderFloat(XORSTR("##STEPMAX"), &aimStepMax, aimStepMin + 1.0f, 35, XORSTR("AimStep max: %0.f")))
						UI::UpdateWeaponSettings();
					ImGui::PopItemWidth();
				}
			}
			ImGui::EndChild();
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, currentWeapon > ItemDefinitionIndex::INVALID &&
				Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end() ? 701 : 736), true);
			{
				ImGui::Text(XORSTR("Aimkey Only"));
				ImGui::Columns(2, nullptr, false);
				{
					if (ImGui::Checkbox(XORSTR("Aimkey"), &aimkeyOnly))
						UI::UpdateWeaponSettings();
				}
				ImGui::NextColumn();
				{
					UI::KeyBindButton(&aimkey);
				}
				ImGui::EndColumns();
				ImGui::Separator();
				ImGui::Text(XORSTR("Other Features"));
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Backtrack"), &Settings::Backtrack::enabled);
					if (ImGui::Checkbox(XORSTR("Target Reacquisition"), &engageLockTR))
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
					if (ImGui::Checkbox(XORSTR("Silent Aim"), &silent))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("No Shoot"), &noShootEnabled))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Auto Slow"), &autoSlow))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Prediction"), &predEnabled))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("EngageLock"), &engageLock))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Distance-Based FOV"), &autoAimRealDistance))
						UI::UpdateWeaponSettings();
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderFloat(XORSTR("##BACKTRACK_TIME"), &Settings::Backtrack::time, 0.0f, Settings::CVarsOverride::fakeLatency ? 0.4f : 0.2f);
					if (Settings::Backtrack::time > (Settings::CVarsOverride::fakeLatency ? 0.4f : 0.2f))
						Settings::Backtrack::time = 0.2f;
					if (ImGui::SliderInt(XORSTR("##TTR"), &engageLockTTR, 0, 1000))
						UI::UpdateWeaponSettings();
					ImGui::PopItemWidth();

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
							if (ImGui::Checkbox(XORSTR("Scope Control"), &scopeControlEnabled))
								UI::UpdateWeaponSettings();
					}
					if (ImGui::Checkbox(XORSTR("Ignore Jump (Self)"), &ignoreJumpEnabled))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Ignore Jump (Enemies)"), &ignoreEnemyJumpEnabled))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Flash Check"), &flashCheck))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Smoke Check"), &smokeCheck))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("FriendlyFire"), &friendly))
						UI::UpdateWeaponSettings();
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
					ImGui::PopItemWidth();
				}
			}
			ImGui::EndColumns();
			ImGui::Separator();
			ImGui::Text(XORSTR("Semirage Options"));
			ImGui::Columns(2, nullptr, false);
			{
				if (ImGui::Checkbox(XORSTR("Auto Shoot"), &autoShootEnabled))
					UI::UpdateWeaponSettings();
			}
			ImGui::NextColumn();
			{
				ImGui::Checkbox(XORSTR("Velocity Check"), &Settings::Legitbot::AutoShoot::velocityCheck);
			}
			ImGui::EndColumns();
			ImGui::Columns(2, nullptr, false);
			{
				if (ImGui::Checkbox(XORSTR("Spread Limit"), &spreadLimitEnabled))
					UI::UpdateWeaponSettings();
				if (ImGui::Checkbox(XORSTR("Hit Chance"), &hitchanceEnabled))
					UI::UpdateWeaponSettings();
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat(XORSTR("##SPREADLIMIT"), &spreadLimit, 0, 0.1))
					UI::UpdateWeaponSettings();
				if (ImGui::SliderFloat(XORSTR("##HITCHANCE"), &hitchanceValue, 0, 100))
					UI::UpdateWeaponSettings();
				ImGui::PopItemWidth();
			}
			ImGui::EndColumns();
			ImGui::Columns(1);
			{
				ImGui::Separator();
				ImGui::Text(XORSTR("Triggerbot"));
				ImGui::Columns(2, nullptr, false);
				{
					if (ImGui::Checkbox(XORSTR("Enabled"), &triggerBotEnabled))
						UI::UpdateWeaponSettings();
					if (ImGui::Checkbox(XORSTR("Hit Chance"), &triggerHitchanceEnabled))
						UI::UpdateWeaponSettings();
					if (ImGui::Button(XORSTR("Filters"), ImVec2(-1, 0)))
						ImGui::OpenPopup(XORSTR("TriggerFilters"));
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					UI::KeyBindButton(&Settings::Triggerbot::key);
					if (ImGui::SliderFloat(XORSTR("##TRIGGERHITCHANCE"), &triggerHitchanceValue, 0, 100, XORSTR("Hitchance: %0.f")))
						UI::UpdateWeaponSettings();
					if (ImGui::Button(XORSTR("Random Delay"), ImVec2(-1, 0)))
						ImGui::OpenPopup(XORSTR("TriggerDelaySettings"));
					ImGui::PopItemWidth();
				}
				ImGui::EndColumns();
				
				ImGui::SetNextWindowSize(ImVec2(235, 145), ImGuiCond_Always);
				if (ImGui::BeginPopup(XORSTR("TriggerFilters"))) {
					ImGui::Text(XORSTR("Filters"));
					ImGui::Columns(2, nullptr, false);
					{
						ImGui::Checkbox(XORSTR("Enemies"), &Settings::Triggerbot::Filters::enemies);
						ImGui::Checkbox(XORSTR("Walls"), &Settings::Triggerbot::Filters::walls);
						ImGui::Checkbox(XORSTR("Head"), &Settings::Triggerbot::Filters::head);
						ImGui::Checkbox(XORSTR("Chest"), &Settings::Triggerbot::Filters::chest);
						ImGui::Checkbox(XORSTR("Legs"), &Settings::Triggerbot::Filters::legs);
					}
					ImGui::NextColumn();
					{
						ImGui::Checkbox(XORSTR("Allies"), &Settings::Triggerbot::Filters::allies);
						ImGui::Checkbox(XORSTR("Smoke check"), &Settings::Triggerbot::Filters::smokeCheck);
						ImGui::Checkbox(XORSTR("Flash check"), &Settings::Triggerbot::Filters::flashCheck);
						ImGui::Checkbox(XORSTR("Stomach"), &Settings::Triggerbot::Filters::stomach);
						ImGui::Checkbox(XORSTR("Arms"), &Settings::Triggerbot::Filters::arms);
					}
					ImGui::Columns(1);
					ImGui::EndPopup();
				}

				ImGui::SetNextWindowSize(ImVec2(275, Settings::Triggerbot::RandomDelay::lastRoll != 0 ? 70 : 60), ImGuiCond_Always);
				if (ImGui::BeginPopup(XORSTR("TriggerDelaySettings"))) {
					ImGui::Columns(2, nullptr, false);
					{
						ImGui::Checkbox(XORSTR("Enabled"), &Settings::Triggerbot::RandomDelay::enabled);
					}
					ImGui::NextColumn();
					{
						ImGui::PushItemWidth(-1);
						if (Settings::Triggerbot::RandomDelay::lastRoll != 0)
							ImGui::Text(XORSTR("Last: %dms"), Settings::Triggerbot::RandomDelay::lastRoll);
						ImGui::SliderInt(XORSTR("##TRIGGERRANDOMLOW"), &Settings::Triggerbot::RandomDelay::lowBound, 5, 220, XORSTR("Minimum ms: %0.f"));
						if (Settings::Triggerbot::RandomDelay::lowBound >= Settings::Triggerbot::RandomDelay::highBound)
							Settings::Triggerbot::RandomDelay::highBound = Settings::Triggerbot::RandomDelay::lowBound + 1;
						ImGui::SliderInt(XORSTR("##TRIGGERRANDOMHIGH"), &Settings::Triggerbot::RandomDelay::highBound, (Settings::Triggerbot::RandomDelay::lowBound + 1), 225, XORSTR("Maximum ms: %0.f"));
						ImGui::PopItemWidth();
					}
					ImGui::Columns(1);
					ImGui::EndPopup();
				}

				ImGui::Text(XORSTR("Auto Knife & Zeus"));
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Auto Knife"), &Settings::AutoKnife::enabled);
					ImGui::Checkbox(XORSTR("On Key"), &Settings::AutoKnife::onKey);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(XORSTR("Enemies"), &Settings::AutoKnife::Filters::enemies);
					ImGui::Checkbox(XORSTR("Allies"), &Settings::AutoKnife::Filters::allies);
				}
				ImGui::EndColumns();
			}
			ImGui::EndChild();
			if (currentWeapon > ItemDefinitionIndex::INVALID && Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end())
			{
				ImGui::BeginChild(XORSTR("Reset"), ImVec2(0, 30), false);
				{
					if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0)))
					{
						Settings::Legitbot::weapons.erase(currentWeapon);
						UI::ReloadWeaponSettings();
					}
					ImGui::Separator();
					ImGui::EndChild();
				}
			}
		}
		ImGui::EndColumns();
	}
	ImGui::EndGroup();
}