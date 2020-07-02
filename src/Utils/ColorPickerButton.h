#include "../settings.h"
#include "xorstring.h"
#include "../ImGUI/imgui_internal.h"
#include "../ATGUI/atgui.h"

struct ColorListVar
{
	const char* name;
    ImVec4 ColorVec4;
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
        this->ColorVec4 = ptr->ColorVec4;
	}

	ColorListVar(const char* name, HealthColorVar* ptr)
	{
		this->name = name;
		this->healthColorVarPtr = ptr;
		this->type = HEALTHCOLORVAR_TYPE;
        this->ColorVec4 = ptr->ColorVec4;
	}
};

namespace ColorButton
{
	void RenderWindow(const char* popupname, const int& ColorIndex, bool show);
    extern void ColorPicker();

    inline ColorListVar colors[] = {
			{ "UI Main", &Settings::UI::mainColor }, // 1
			{ "UI Body", &Settings::UI::bodyColor }, //2
			{ "UI Font", &Settings::UI::fontColor }, //3 
			{ "UI Accent", &Settings::UI::accentColor }, //4
			{ "FOV Circle", &Settings::ESP::FOVCrosshair::color }, //5
 			{ "Spread", &Settings::ESP::Spread::color }, // 6
			{ "SpreadLimit", &Settings::ESP::Spread::spreadLimitColor}, //7
			{ "Hitmarker", &Settings::ESP::Hitmarker::color }, // 8
			{ "ESP - Enemy", &Settings::ESP::enemyColor }, //9
			{ "ESP - Team", &Settings::ESP::allyColor }, //10
			{ "ESP - Enemy Visible", &Settings::ESP::enemyVisibleColor }, //11
			{ "ESP - Team Visible", &Settings::ESP::allyVisibleColor }, //12
			{ "ESP - CT", &Settings::ESP::ctColor }, //13
			{ "ESP - T", &Settings::ESP::tColor }, //14
			{ "ESP - CT Visible", &Settings::ESP::ctVisibleColor }, //15
			{ "ESP - T Visible", &Settings::ESP::tVisibleColor }, // 16
			{ "ESP - LocalPlayer", &Settings::ESP::localplayerColor }, //17
			{ "ESP - Bomb", &Settings::ESP::bombColor }, //18
			{ "ESP - Bomb Defusing", &Settings::ESP::bombDefusingColor }, //19
			{ "ESP - Hostage", &Settings::ESP::hostageColor }, //20
			{ "ESP - Defuser", &Settings::ESP::defuserColor }, //21
			{ "ESP - Weapon", &Settings::ESP::weaponColor }, // 22
			{ "ESP - Chicken", &Settings::ESP::chickenColor }, //23 
			{ "ESP - Fish", &Settings::ESP::fishColor }, //24
			{ "ESP - Smoke", &Settings::ESP::smokeColor }, // 25
			{ "ESP - Decoy", &Settings::ESP::decoyColor }, // 26
			{ "ESP - Flashbang", &Settings::ESP::flashbangColor }, // 27
			{ "ESP - Grenade", &Settings::ESP::grenadeColor }, // 28
			{ "ESP - Molotov", &Settings::ESP::molotovColor }, // 29
			{ "ESP - Bump Mine", &Settings::ESP::mineColor }, // 30
			{ "ESP - Breach Charge", &Settings::ESP::chargeColor }, // 31
			{ "ESP - Skeleton (Team)", &Settings::ESP::Skeleton::allyColor }, // 32
			{ "ESP - Skeleton (Enemy)", &Settings::ESP::Skeleton::enemyColor }, // 33
			{ "ESP - Player Info (Team)", &Settings::ESP::allyInfoColor }, // 34
			{ "ESP - Player Info (Enemy)", &Settings::ESP::enemyInfoColor }, // 35
			{ "ESP - Danger Zone: Weapon Upgrade", &Settings::ESP::DangerZone::upgradeColor }, // 36
			{ "ESP - Danger Zone: Loot Crate", &Settings::ESP::DangerZone::lootcrateColor }, // 37
			{ "ESP - Danger Zone: Radar Jammer", &Settings::ESP::DangerZone::radarjammerColor }, // 38
			{ "ESP - Danger Zone: Explosive Barrel", &Settings::ESP::DangerZone::barrelColor }, // 39
			{ "ESP - Danger Zone: Ammo Box", &Settings::ESP::DangerZone::ammoboxColor }, // 40
			{ "ESP - Danger Zone: Safe", &Settings::ESP::DangerZone::safeColor }, // 41
			{ "ESP - Danger Zone: Sentry Turret", &Settings::ESP::DangerZone::dronegunColor }, // 42
			{ "ESP - Danger Zone: Drone", &Settings::ESP::DangerZone::droneColor }, //43
			{ "ESP - Danger Zone: Cash", &Settings::ESP::DangerZone::cashColor }, //44
			{ "ESP - Danger Zone: Tablet", &Settings::ESP::DangerZone::tabletColor }, //45
			{ "ESP - Danger Zone: Healthshot", &Settings::ESP::DangerZone::healthshotColor }, // 46
			{ "ESP - Danger Zone: Melee", &Settings::ESP::DangerZone::meleeColor }, // 47
			{ "Chams - Team", &Settings::ESP::Chams::allyColor }, // 48
			{ "Chams - Team Visible", &Settings::ESP::Chams::allyVisibleColor }, // 49
			{ "Chams - Enemy Visible", &Settings::ESP::Chams::enemyVisibleColor }, // 50
			{ "Chams - Enemy", &Settings::ESP::Chams::enemyColor }, // 51
			{ "Chams - LocalPlayer", &Settings::ESP::Chams::localplayerColor }, // 52
			{ "Chams - Arms", &Settings::ESP::Chams::Arms::color }, // 53
			{ "Chams - Weapon", &Settings::ESP::Chams::Weapon::color }, // 54
			{ "Grenade Helper - Aim Line", &Settings::GrenadeHelper::aimLine }, // 55
			{ "Grenade Helper - Aim Dot", &Settings::GrenadeHelper::aimDot }, // 56
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
}



