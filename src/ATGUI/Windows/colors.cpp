#include "colors.h"

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

bool Colors::showWindow = false;

void Colors::RenderWindow()
{
	if( Settings::UI::Windows::Colors::reload )
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Colors::posX, Settings::UI::Windows::Colors::posY), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Colors::sizeX, Settings::UI::Windows::Colors::sizeY), ImGuiSetCond_Always);
		Settings::UI::Windows::Colors::reload = false;
		Colors::showWindow = Settings::UI::Windows::Colors::open;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Colors::posX, Settings::UI::Windows::Colors::posY), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Colors::sizeX, Settings::UI::Windows::Colors::sizeY), ImGuiSetCond_FirstUseEver);
	}
	if (!Colors::showWindow)
	{
		Settings::UI::Windows::Colors::open = false;
		return;
	}

	struct ColorListVar
	{
		const char* name;
		union
		{
			ColorVar* colorVarPtr;
			HealthColorVar* healthColorVarPtr;
		};
		enum
		{
			COLORVAR_TYPE,
			HEALTHCOLORVAR_TYPE
		} type;

		ColorListVar(const char* name, ColorVar* ptr)
		{
			this->name = name;
			this->colorVarPtr = ptr;
			this->type = COLORVAR_TYPE;
		}

		ColorListVar(const char* name, HealthColorVar* ptr)
		{
			this->name = name;
			this->healthColorVarPtr = ptr;
			this->type = HEALTHCOLORVAR_TYPE;
		}
	};

	ColorListVar colors[] = {
			{ "UI Main", &Settings::UI::mainColor },
			{ "UI Body", &Settings::UI::bodyColor },
			{ "UI Font", &Settings::UI::fontColor },
			{ "UI Accent", &Settings::UI::accentColor },
			{ "FOV Circle", &Settings::ESP::FOVCrosshair::color },
			{ "Spread", &Settings::ESP::Spread::color },
			{ "SpreadLimit", &Settings::ESP::Spread::spreadLimitColor},
			{ "Hitmarker", &Settings::ESP::Hitmarker::color },
			{ "ESP - Enemy", &Settings::ESP::enemyColor },
			{ "ESP - Team", &Settings::ESP::allyColor },
			{ "ESP - Enemy Visible", &Settings::ESP::enemyVisibleColor },
			{ "ESP - Team Visible", &Settings::ESP::allyVisibleColor },
			{ "ESP - CT", &Settings::ESP::ctColor },
			{ "ESP - T", &Settings::ESP::tColor },
			{ "ESP - CT Visible", &Settings::ESP::ctVisibleColor },
			{ "ESP - T Visible", &Settings::ESP::tVisibleColor },
			{ "ESP - LocalPlayer", &Settings::ESP::localplayerColor },
			{ "ESP - Bomb", &Settings::ESP::bombColor },
			{ "ESP - Bomb Defusing", &Settings::ESP::bombDefusingColor },
			{ "ESP - Hostage", &Settings::ESP::hostageColor },
			{ "ESP - Defuser", &Settings::ESP::defuserColor },
			{ "ESP - Weapon", &Settings::ESP::weaponColor },
			{ "ESP - Chicken", &Settings::ESP::chickenColor },
			{ "ESP - Fish", &Settings::ESP::fishColor },
			{ "ESP - Smoke", &Settings::ESP::smokeColor },
			{ "ESP - Decoy", &Settings::ESP::decoyColor },
			{ "ESP - Flashbang", &Settings::ESP::flashbangColor },
			{ "ESP - Grenade", &Settings::ESP::grenadeColor },
			{ "ESP - Molotov", &Settings::ESP::molotovColor },
			{ "ESP - Bump Mine", &Settings::ESP::mineColor },
			{ "ESP - Breach Charge", &Settings::ESP::chargeColor },
			{ "ESP - Skeleton (Team)", &Settings::ESP::Skeleton::allyColor },
			{ "ESP - Skeleton (Enemy)", &Settings::ESP::Skeleton::enemyColor },
			{ "ESP - Player Info (Team)", &Settings::ESP::allyInfoColor },
			{ "ESP - Player Info (Enemy)", &Settings::ESP::enemyInfoColor },
			{ "ESP - Danger Zone: Weapon Upgrade", &Settings::ESP::DangerZone::upgradeColor },
			{ "ESP - Danger Zone: Loot Crate", &Settings::ESP::DangerZone::lootcrateColor },
			{ "ESP - Danger Zone: Radar Jammer", &Settings::ESP::DangerZone::radarjammerColor },
			{ "ESP - Danger Zone: Explosive Barrel", &Settings::ESP::DangerZone::barrelColor },
			{ "ESP - Danger Zone: Ammo Box", &Settings::ESP::DangerZone::ammoboxColor },
			{ "ESP - Danger Zone: Safe", &Settings::ESP::DangerZone::safeColor },
			{ "ESP - Danger Zone: Sentry Turret", &Settings::ESP::DangerZone::dronegunColor },
			{ "ESP - Danger Zone: Drone", &Settings::ESP::DangerZone::droneColor },
			{ "ESP - Danger Zone: Cash", &Settings::ESP::DangerZone::cashColor },
			{ "ESP - Danger Zone: Tablet", &Settings::ESP::DangerZone::tabletColor },
			{ "ESP - Danger Zone: Healthshot", &Settings::ESP::DangerZone::healthshotColor },
			{ "ESP - Danger Zone: Melee", &Settings::ESP::DangerZone::meleeColor },
			{ "Chams - Team", &Settings::ESP::Chams::allyColor },
			{ "Chams - Team Visible", &Settings::ESP::Chams::allyVisibleColor },
			{ "Chams - Enemy", &Settings::ESP::Chams::enemyColor },
			{ "Chams - Enemy Visible", &Settings::ESP::Chams::enemyVisibleColor },
			{ "Chams - LocalPlayer", &Settings::ESP::Chams::localplayerColor },
			{ "Chams - Arms", &Settings::ESP::Chams::Arms::color },
			{ "Chams - Weapon", &Settings::ESP::Chams::Weapon::color },
			{ "Grenade Helper - Aim Line", &Settings::GrenadeHelper::aimLine },
			{ "Grenade Helper - Aim Dot", &Settings::GrenadeHelper::aimDot },
			{ "Grenade Helper - HE Info", &Settings::GrenadeHelper::infoHE },
			{ "Grenade Helper - Smoke Info", &Settings::GrenadeHelper::infoSmoke },
			{ "Grenade Helper - Molotov Info", &Settings::GrenadeHelper::infoMolotov },
			{ "Grenade Helper - Flash Info", &Settings::GrenadeHelper::infoFlash },
			{ "Grenade Prediction - Line", &Settings::GrenadePrediction::color },
			{ "Event log", &Settings::Eventlog::color },
			{ "Radar - Enemy", &Settings::Radar::enemyColor },
			{ "Radar - Team", &Settings::Radar::allyColor },
			{ "Radar - Enemy Visible", &Settings::Radar::enemyVisibleColor },
			{ "Radar - Team Visible", &Settings::Radar::allyVisibleColor },
			{ "Radar - CT", &Settings::Radar::ctColor },
			{ "Radar - T", &Settings::Radar::tColor },
			{ "Radar - CT Visible", &Settings::Radar::ctVisibleColor },
			{ "Radar - T Visible", &Settings::Radar::tVisibleColor },
			{ "Radar - Bomb", &Settings::Radar::bombColor },
			{ "Radar - Bomb Defusing", &Settings::Radar::bombDefusingColor },
			{ "Glow - Team", &Settings::ESP::Glow::allyColor },
			{ "Glow - Enemy", &Settings::ESP::Glow::enemyColor },
			{ "Glow - Enemy Visible", &Settings::ESP::Glow::enemyVisibleColor },
			{ "Glow - LocalPlayer", &Settings::ESP::Glow::localplayerColor },
			{ "Glow - Weapon", &Settings::ESP::Glow::weaponColor },
			{ "Glow - Grenade", &Settings::ESP::Glow::grenadeColor },
			{ "Glow - Defuser", &Settings::ESP::Glow::defuserColor },
			{ "Glow - Chicken", &Settings::ESP::Glow::chickenColor },
			{ "Sky", &Settings::NoSky::color },
			{ "Walls", &Settings::ASUSWalls::color },
	};

	const char* colorNames[IM_ARRAYSIZE(colors)];
	for (int i = 0; i < IM_ARRAYSIZE(colors); i++)
		colorNames[i] = colors[i].name;

	static int colorSelected = 0;

	if (ImGui::Begin(XORSTR("Colors"), &Colors::showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
	{
		Settings::UI::Windows::Colors::open = true;
		ImVec2 temp = ImGui::GetWindowSize();
		Settings::UI::Windows::Colors::sizeX = (int)temp.x;
		Settings::UI::Windows::Colors::sizeY = (int)temp.y;
		temp = ImGui::GetWindowPos();
		Settings::UI::Windows::Colors::posX = (int)temp.x;
		Settings::UI::Windows::Colors::posY = (int)temp.y;
		ImGui::Columns(2, nullptr, true);
		{
			ImGui::PushItemWidth(-1);
			ImGui::ListBox(XORSTR("##COLORSELECTION"), &colorSelected, colorNames, IM_ARRAYSIZE(colorNames), 12);
			ImGui::PopItemWidth();
		}
		ImGui::NextColumn();
		{
			if (colors[colorSelected].type == ColorListVar::HEALTHCOLORVAR_TYPE)
			{
				UI::ColorPicker4((float*)colors[colorSelected].healthColorVarPtr);
				ImGui::Checkbox(XORSTR("Rainbow"), &colors[colorSelected].healthColorVarPtr->rainbow);
				ImGui::SameLine();
				ImGui::Checkbox(XORSTR("Health-Based"), &colors[colorSelected].healthColorVarPtr->hp);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &colors[colorSelected].healthColorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
				ImGui::PopItemWidth();
			}
			else
			{
				UI::ColorPicker4((float*)colors[colorSelected].colorVarPtr);
				ImGui::Checkbox(XORSTR("Rainbow"), &colors[colorSelected].colorVarPtr->rainbow);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &colors[colorSelected].colorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
				ImGui::PopItemWidth();
			}
		}
		ImGui::End();
	}
}
