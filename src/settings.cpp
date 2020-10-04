#include "settings.h"

#include <dirent.h>
#include <fstream>
#include <unistd.h>

#include "ATGUI/atgui.h"
#include "Hacks/clantagchanger.h"
#include "Hacks/esp.h"
#include "Hacks/skinchanger.h"
#include "Hacks/tracereffect.h"
#include "Utils/draw.h"
#include "Utils/util.h"
#include "Utils/util_items.h"
#include "Utils/util_sdk.h"
#include "Utils/xorstring.h"
#include "config.h"
#include "fonts.h"
#include "interfaces.h"
#include "json/json.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void GetVal(Json::Value& config, int* setting)
{
    if (config.isNull())
		return;

    *setting = config.asInt();
}

void GetVal(Json::Value& config, bool* setting)
{
    if (config.isNull())
		return;

    *setting = config.asBool();
}

void GetVal(Json::Value& config, float* setting)
{
    if (config.isNull())
	    return;

    *setting = config.asFloat();
}

void GetVal(Json::Value& config, ImColor* setting)
{
    if (config.isNull())
	return;

    GetVal(config[XORSTR("r")], &setting->Value.x);
    GetVal(config[XORSTR("g")], &setting->Value.y);
    GetVal(config[XORSTR("b")], &setting->Value.z);
    GetVal(config[XORSTR("a")], &setting->Value.w);
}

void GetVal(Json::Value& config, char** setting)
{
    if (config.isNull())
	return;

    *setting = strdup(config.asCString());
}

void GetVal(Json::Value& config, char* setting)
{
    if (config.isNull())
	return;

    strcpy(setting, config.asCString());
}

void GetVal(Json::Value& config, ColorVar* setting)
{
    if (config.isNull())
	return;

    GetVal(config[XORSTR("r")], &setting->color.Value.x);
    GetVal(config[XORSTR("g")], &setting->color.Value.y);
    GetVal(config[XORSTR("b")], &setting->color.Value.z);
    GetVal(config[XORSTR("a")], &setting->color.Value.w);
    GetVal(config[XORSTR("rainbow")], &setting->rainbow);
    GetVal(config[XORSTR("rainbowSpeed")], &setting->rainbowSpeed);
}

void GetVal(Json::Value& config, HealthColorVar* setting)
{
    if (config.isNull())
	return;

    GetVal(config[XORSTR("r")], &setting->color.Value.x);
    GetVal(config[XORSTR("g")], &setting->color.Value.y);
    GetVal(config[XORSTR("b")], &setting->color.Value.z);
    GetVal(config[XORSTR("a")], &setting->color.Value.w);
    GetVal(config[XORSTR("rainbow")], &setting->rainbow);
    GetVal(config[XORSTR("rainbowSpeed")], &setting->rainbowSpeed);
    GetVal(config[XORSTR("hp")], &setting->hp);
}

template <typename Ord, Ord (*lookupFunction)(std::string)>
void GetOrdinal(Json::Value& config, Ord* setting)
{
    if (config.isNull())
	return;

    Ord value;
    if (config.isString())
	value = lookupFunction(config.asString());
    else
	value = (Ord)config.asInt();

    *setting = value;
}

void GetButtonCode(Json::Value& config, enum ButtonCode_t* setting)
{
    GetOrdinal<enum ButtonCode_t, Util::GetButtonCode>(config, setting);
}

void LoadColor(Json::Value& config, ImColor color)
{
    config[XORSTR("r")] = color.Value.x;
    config[XORSTR("g")] = color.Value.y;
    config[XORSTR("b")] = color.Value.z;
    config[XORSTR("a")] = color.Value.w;
}

void LoadColor(Json::Value& config, ColorVar color)
{
    config[XORSTR("r")] = color.color.Value.x;
    config[XORSTR("g")] = color.color.Value.y;
    config[XORSTR("b")] = color.color.Value.z;
    config[XORSTR("a")] = color.color.Value.w;
    config[XORSTR("rainbow")] = color.rainbow;
    config[XORSTR("rainbowSpeed")] = color.rainbowSpeed;
}

void LoadColor(Json::Value& config, HealthColorVar color)
{
    config[XORSTR("r")] = color.color.Value.x;
    config[XORSTR("g")] = color.color.Value.y;
    config[XORSTR("b")] = color.color.Value.z;
    config[XORSTR("a")] = color.color.Value.w;
    config[XORSTR("rainbow")] = color.rainbow;
    config[XORSTR("rainbowSpeed")] = color.rainbowSpeed;
    config[XORSTR("hp")] = color.hp;
}

void Settings::LoadDefaultsOrSave(std::string path)
{
    Json::Value settings;
    Json::StyledWriter styledWriter;

    LoadColor(settings[XORSTR("UI")][XORSTR("mainColor")], Settings::UI::mainColor);
    LoadColor(settings[XORSTR("UI")][XORSTR("bodyColor")], Settings::UI::bodyColor);
    LoadColor(settings[XORSTR("UI")][XORSTR("fontColor")], Settings::UI::fontColor);
    LoadColor(settings[XORSTR("UI")][XORSTR("accentColor")], Settings::UI::accentColor);
    settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("family")] = Settings::UI::Fonts::ESP::family;
    settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("size")] = Settings::UI::Fonts::ESP::size;
    settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("flags")] = Settings::UI::Fonts::ESP::flags;

    for (auto i : Settings::Legitbot::weapons)
    {
    // TODO this is kind of a hack and i'm too tired to find a better way to do this
    // yes i tried defining a variable, skinSetting, and giving it the same value but woooooo operator overloading
    // in C++ and weird shit
    #define LegitweaponSetting settings[XORSTR("Legitbot")][XORSTR("weapons")][Util::Items::GetItemName((enum ItemDefinitionIndex)i.first)]
	    LegitweaponSetting[XORSTR("Silent")] = i.second.silent;
	    LegitweaponSetting[XORSTR("AutoShoot")] = i.second.autoShoot;
	    LegitweaponSetting[XORSTR("TargetBone")] = (int)i.second.bone;
	    LegitweaponSetting[XORSTR("AimKey")] = Util::GetButtonName(i.second.aimkey);
	    LegitweaponSetting[XORSTR("AimKeyOnly")] = i.second.aimkeyOnly;
	    LegitweaponSetting[XORSTR("Smooth")][XORSTR("Enabled")] = i.second.smoothEnabled;
	    LegitweaponSetting[XORSTR("Smooth")][XORSTR("Amount")] = i.second.smoothAmount;
	    LegitweaponSetting[XORSTR("Smooth")][XORSTR("Type")] = (int)i.second.smoothType;
	    LegitweaponSetting[XORSTR("Smooth")][XORSTR("Salting")][XORSTR("Enabled")] = i.second.smoothSaltEnabled;
	    LegitweaponSetting[XORSTR("Smooth")][XORSTR("Salting")][XORSTR("Multiplier")] = i.second.smoothSaltMultiplier;
	    LegitweaponSetting[XORSTR("ErrorMargin")][XORSTR("Enabled")] = i.second.errorMarginEnabled;
	    LegitweaponSetting[XORSTR("ErrorMargin")][XORSTR("Value")] = i.second.errorMarginValue;
	    LegitweaponSetting[XORSTR("AutoAim")][XORSTR("Enabled")] = i.second.autoAimEnabled;
	    LegitweaponSetting[XORSTR("AutoAim")][XORSTR("LegitFOV")] = i.second.LegitautoAimFov;
	    LegitweaponSetting[XORSTR("AimStep")][XORSTR("Enabled")] = i.second.aimStepEnabled;
	    LegitweaponSetting[XORSTR("AimStep")][XORSTR("min")] = i.second.aimStepMin;
	    LegitweaponSetting[XORSTR("AimStep")][XORSTR("max")] = i.second.aimStepMax;
	    LegitweaponSetting[XORSTR("RCS")][XORSTR("Enabled")] = i.second.rcsEnabled;
	    LegitweaponSetting[XORSTR("RCS")][XORSTR("AlwaysOn")] = i.second.rcsAlwaysOn;
	    LegitweaponSetting[XORSTR("RCS")][XORSTR("AmountX")] = i.second.rcsAmountX;
    	LegitweaponSetting[XORSTR("RCS")][XORSTR("AmountY")] = i.second.rcsAmountY;
    	LegitweaponSetting[XORSTR("AutoPistol")][XORSTR("Enabled")] = i.second.autoPistolEnabled;
    	LegitweaponSetting[XORSTR("AutoScope")][XORSTR("Enabled")] = i.second.autoScopeEnabled;
    	LegitweaponSetting[XORSTR("IgnoreJump")][XORSTR("Enabled")] = i.second.ignoreJumpEnabled;
	    LegitweaponSetting[XORSTR("IgnoreEnemyJump")][XORSTR("Enabled")] = i.second.ignoreEnemyJumpEnabled;
    	LegitweaponSetting[XORSTR("HitChance")][XORSTR("Enabled")] = i.second.hitchanceEnaled;
	    LegitweaponSetting[XORSTR("HitChance")][XORSTR("Value")] = i.second.hitchance;
    	LegitweaponSetting[XORSTR("AutoSlow")][XORSTR("enabled")] = i.second.autoSlow;
	    LegitweaponSetting[XORSTR("Prediction")][XORSTR("enabled")] = i.second.predEnabled;
        LegitweaponSetting[XORSTR("TriggerBot")][XORSTR("enabled")] = i.second.TriggerBot;
        LegitweaponSetting[XORSTR("MinDamage")][XORSTR("enabled")] = i.second.mindamage;
        LegitweaponSetting[XORSTR("MinDamage")][XORSTR("Value")] = i.second.minDamagevalue;
        LegitweaponSetting[XORSTR("Autowall")][XORSTR("enabled")] = i.second.autoWall;
	
	    for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
	        LegitweaponSetting[XORSTR("DesiredBones")][XORSTR("Bones")][bone] = i.second.desiredBones[bone];

    #undef LegitweaponSetting
    }

    for (auto i : Settings::Ragebot::weapons)
    {
    // TODO this is kind of a hack and i'm too tired to find a better way to do this
    // yes i tried defining a variable, skinSetting, and giving it the same value but woooooo operator overloading
    // in C++ and weird shit
    #define RageweaponSetting settings[XORSTR("Ragebot")][XORSTR("weapons")][Util::Items::GetItemName((enum ItemDefinitionIndex)i.first)]
	    RageweaponSetting[XORSTR("Silent")] = i.second.silent;
	    RageweaponSetting[XORSTR("Friendly")] = i.second.friendly;
	    RageweaponSetting[XORSTR("ClosestBone")] = i.second.closestBone;
	    RageweaponSetting[XORSTR("AutoPistol")][XORSTR("Enabled")] = i.second.autoPistolEnabled;
	    RageweaponSetting[XORSTR("AutoShoot")][XORSTR("Enabled")] = i.second.autoShootEnabled;
	    RageweaponSetting[XORSTR("AutoScope")][XORSTR("Enabled")] = i.second.autoScopeEnabled;
	    RageweaponSetting[XORSTR("HitChance")][XORSTR("Enabled")] = i.second.HitChanceEnabled;
	    RageweaponSetting[XORSTR("HitChance")][XORSTR("Value")] = i.second.HitChance;
        RageweaponSetting[XORSTR("MinDamage")] = i.second.MinDamage;
	    RageweaponSetting[XORSTR("AutoSlow")][XORSTR("Enabled")] = i.second.autoSlow;
	    RageweaponSetting[XORSTR("DoubleFire")][XORSTR("Enabled")] = i.second.DoubleFire;
	    RageweaponSetting[XORSTR("ScopeControl")][XORSTR("Enabled")] = i.second.scopeControlEnabled;
        RageweaponSetting[XORSTR("DamagePrediction")][XORSTR("Type")] = (int)i.second.DmagePredictionType;
        RageweaponSetting[XORSTR("EnemySelectionType")][XORSTR("Type")] = (int)i.second.enemySelectionType;

        for (int bone = 0; bone < 6; bone++)
        {
            RageweaponSetting[XORSTR("DesireBones")][XORSTR("Bones")][bone] = i.second.desireBones[bone];
            RageweaponSetting[XORSTR("DesireMultiBones")][XORSTR("Bones")][bone] = i.second.desiredMultiBones[bone];
        }
	
    #undef RageweaponSetting
    }

    settings[XORSTR("Legitbot")][XORSTR("AutoCrouch")][XORSTR("enabled")] = Settings::Legitbot::AutoCrouch::enabled;
    settings[XORSTR("Ragebot")][XORSTR("AUtoAcrouth")][XORSTR("enable")] = Settings::Ragebot::AutoCrouch::enable;
    settings[XORSTR("Ragebot")][XORSTR("BackTrack")][XORSTR("enable")] = Settings::Ragebot::backTrack::enabled;
    settings[XORSTR("Ragebot")][XORSTR("BackTrack")][XORSTR("time")] = Settings::Ragebot::backTrack::time;
    settings[XORSTR("Ragebot")][XORSTR("quickpeek")][XORSTR("key")] = Util::GetButtonName(Settings::Ragebot::quickpeek::key);
    settings[XORSTR("Ragebot")][XORSTR("quickpeek")][XORSTR("enabled")] = Settings::Ragebot::quickpeek::enabled;
    settings[XORSTR("Triggerbot")][XORSTR("enabled")] = Settings::Triggerbot::enabled;
	settings[XORSTR("Triggerbot")][XORSTR("key")] = Util::GetButtonName(Settings::Triggerbot::key);
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("enemies")] = Settings::Triggerbot::Filters::enemies;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("allies")] = Settings::Triggerbot::Filters::allies;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("walls")] = Settings::Triggerbot::Filters::walls;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("smoke_check")] = Settings::Triggerbot::Filters::smokeCheck;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("flash_check")] = Settings::Triggerbot::Filters::flashCheck;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("head")] = Settings::Triggerbot::Filters::head;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("chest")] = Settings::Triggerbot::Filters::chest;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("stomach")] = Settings::Triggerbot::Filters::stomach;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("arms")] = Settings::Triggerbot::Filters::arms;
	settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("legs")] = Settings::Triggerbot::Filters::legs;
	settings[XORSTR("Triggerbot")][XORSTR("RandomDelay")][XORSTR("enabled")] = Settings::Triggerbot::RandomDelay::enabled;
	settings[XORSTR("Triggerbot")][XORSTR("RandomDelay")][XORSTR("lowBound")] = Settings::Triggerbot::RandomDelay::lowBound;
	settings[XORSTR("Triggerbot")][XORSTR("RandomDelay")][XORSTR("highBound")] = Settings::Triggerbot::RandomDelay::highBound;

	settings[XORSTR("AntiAim")][XORSTR("pitchtype")] = (int) Settings::AntiAim::pitchtype;
    settings[XORSTR("AntiAim")][XORSTR("Type")] = (int)Settings::AntiAim::Type::antiaimType;
    settings[XORSTR("AntiAim")][XORSTR("AutoDisable")][XORSTR("no_enemy")] = Settings::AntiAim::AutoDisable::noEnemy;
    settings[XORSTR("AntiAim")][XORSTR("AutoDisable")][XORSTR("knife_held")] = Settings::AntiAim::AutoDisable::knifeHeld;
    settings[XORSTR("AntiAim")][XORSTR("FakeDuck")][XORSTR("enabled")] = Settings::AntiAim::FakeDuck::enabled;
    settings[XORSTR("AntiAim")][XORSTR("FakeDuck")][XORSTR("fakeDuckKey")] = Util::GetButtonName(Settings::AntiAim::FakeDuck::fakeDuckKey);

    settings[XORSTR("SilentWalk")][XORSTR("Key")] = Util::GetButtonName(Settings::SilentWalk::key);
    settings[XORSTR("SilentWalk")][XORSTR("enabled")] = Settings::SilentWalk::enabled;


    settings[XORSTR("AntiAim")][XORSTR("airspin")][XORSTR("enabled")] = Settings::AntiAim::airspin::enabled;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("enabled")] = Settings::AntiAim::RageAntiAim::enable;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("invertonhurt")] = Settings::AntiAim::RageAntiAim::invertOnHurt;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("Fake Percent")] = Settings::AntiAim::RageAntiAim::AntiAImPercent;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("Real Jitter Percent")] = Settings::AntiAim::RageAntiAim::JitterPercent;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("togglekey")] = Util::GetButtonName(Settings::AntiAim::RageAntiAim::InvertKey);
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("inverted")] = Settings::AntiAim::RageAntiAim::inverted;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("Type")] = (int)Settings::AntiAim::RageAntiAim::Type;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("LbyMode")] = (int)Settings::AntiAim::RageAntiAim::lbym;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("atTheTarget")] = Settings::AntiAim::RageAntiAim::atTheTarget;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("SendReal")] = Settings::AntiAim::RageAntiAim::SendReal;
    settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("Enable")] = Settings::AntiAim::ManualAntiAim::Enable;
    settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("backButton")] = Settings::AntiAim::ManualAntiAim::backButton;
    settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("RightButton")] = Settings::AntiAim::ManualAntiAim::RightButton;
    settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("LeftButton")] = Settings::AntiAim::ManualAntiAim::LeftButton;

    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("offset")] = Settings::AntiAim::RageAntiAim::offset;
    settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("pitchJitter")] = Settings::AntiAim::RageAntiAim::pitchJitter;
    settings[XORSTR("AntiAim")][XORSTR("LBYJitter")] = Settings::AntiAim::lbyjitter;

    settings[XORSTR("SvCheats")][XORSTR("enabled")] = Settings::SvCheats::enabled;
    settings[XORSTR("SvCheatss")][XORSTR("enabled")] = Settings::SvCheats::svcheats::enabled;

    settings[XORSTR("gravity")][XORSTR("enabled")] = Settings::SvCheats::gravity::enabled;
    settings[XORSTR("impacts")][XORSTR("enabled")] = Settings::SvCheats::impacts::enabled;
    settings[XORSTR("grenadetraj")][XORSTR("enabled")] = Settings::SvCheats::grenadetraj::enabled;
    settings[XORSTR("viewmodel")][XORSTR("enabled")] = Settings::SvCheats::viewmodel::enabled;
    settings[XORSTR("viewmodelfov")][XORSTR("fov")] = Settings::SvCheats::viewmodel::fov;

    settings[XORSTR("aspect")][XORSTR("enabled")] = Settings::SvCheats::aspect::enabled;
    settings[XORSTR("aspect")][XORSTR("var")] = Settings::SvCheats::aspect::var;

    settings[XORSTR("gravity")][XORSTR("enabled")] = Settings::SvCheats::gravity::enabled;
    settings[XORSTR("gravity")][XORSTR("amount")] = Settings::SvCheats::gravity::amount;


    settings[XORSTR("bright")][XORSTR("enabled")] = Settings::SvCheats::bright::enabled;
    settings[XORSTR("fog")][XORSTR("enabled")] = Settings::SvCheats::fog::enabled;

    settings[XORSTR("viewmodelx")][XORSTR("x")] = Settings::SvCheats::viewmodel::x;
    settings[XORSTR("viewmodely")][XORSTR("y")] = Settings::SvCheats::viewmodel::y;
    settings[XORSTR("viewmodelz")][XORSTR("z")] = Settings::SvCheats::viewmodel::z;

    settings[XORSTR("BuyBot")][XORSTR("enabled")] = Settings::buybot::enabled;
    settings[XORSTR("BuyBot")][XORSTR("scout")] = Settings::buybot::scout;
    settings[XORSTR("BuyBot")][XORSTR("autosniper")] = Settings::buybot::autosniper;
    /*
    * legit anti aim settings 
    */
    settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("enabled")] = Settings::AntiAim::LegitAntiAim::enable;
    settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("togglekey")] = Util::GetButtonName(Settings::AntiAim::LegitAntiAim::InvertKey);
    settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("inverted")] = Settings::AntiAim::LegitAntiAim::inverted;
    // settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("OverWatchProof")] = (int)Settings::AntiAim::LegitAntiAim::legitAAtype;
    // END

	settings[XORSTR("AntiAim")][XORSTR("Yaw")][XORSTR("type")] = (int) Settings::AntiAim::Yaw::typeReal;
	settings[XORSTR("AntiAim")][XORSTR("Yaw")][XORSTR("type_fake")] = (int) Settings::AntiAim::Yaw::typeFake;
    settings[XORSTR("AntiAim")][XORSTR("HeadEdge")][XORSTR("enabled")] = Settings::AntiAim::HeadEdge::enabled;
    settings[XORSTR("AntiAim")][XORSTR("HeadEdge")][XORSTR("distance")] = Settings::AntiAim::HeadEdge::distance;
    settings[XORSTR("AntiAim")][XORSTR("LBYBreaker")][XORSTR("enabled")] = Settings::AntiAim::LBYBreaker::enabled;
    settings[XORSTR("AntiAim")][XORSTR("LBYBreaker")][XORSTR("offset")] = Settings::AntiAim::LBYBreaker::offset;
    settings[XORSTR("Resolver")][XORSTR("resolve_allAP")] = Settings::Resolver::resolveAllAP;
    settings[XORSTR("Resolver")][XORSTR("resolve_all")] = Settings::Resolver::resolveAll;
    settings[XORSTR("Resolver")][XORSTR("manual")] = Settings::Resolver::manual;
    settings[XORSTR("Resolver")][XORSTR("goalFeetYaw")] = Settings::Resolver::goalFeetYaw;
    settings[XORSTR("Resolver")][XORSTR("EyeAngles")] = Settings::Resolver::EyeAngles;
    settings[XORSTR("Resolver")][XORSTR("rPitch")] = Settings::Resolver::rPitch;

    LoadColor(settings[XORSTR("ESP")][XORSTR("manualAAColor")], Settings::ESP::manualAAColor);
    settings[XORSTR("Resolver")][XORSTR("Type")] = (int)Settings::Resolver::resolverType;

    settings[XORSTR("Ragebot")][XORSTR("impactType")] = (int)Settings::Ragebot::impacttype;

    settings[XORSTR("UI")][XORSTR("particles")] = Settings::UI::particles;

    settings[XORSTR("ESP")][XORSTR("KeybindsY")] = Settings::ESP::keybi::y;
    settings[XORSTR("ESP")][XORSTR("KeybindsX")] = Settings::ESP::keybi::x;
    settings[XORSTR("ESP")][XORSTR("showKeybinds")] = Settings::ESP::KeyBinds;
    settings[XORSTR("ESP")][XORSTR("showDormant")] = Settings::ESP::showDormant;
    settings[XORSTR("ESP")][XORSTR("showimpacts")] = Settings::ESP::showimpacts;
    settings[XORSTR("ESP")][XORSTR("enabled")] = Settings::ESP::enabled;
    settings[XORSTR("ESP")][XORSTR("backend")] = (int)Settings::ESP::backend;
    settings[XORSTR("ESP")][XORSTR("key")] = Util::GetButtonName(Settings::ESP::key);
    LoadColor(settings[XORSTR("Ragebot")][XORSTR("quickpeek")][XORSTR("color")], Settings::Ragebot::quickpeek::color);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Watermark")][XORSTR("color")], Settings::ESP::Watermark::color);
    LoadColor(settings[XORSTR("ESP")][XORSTR("enemy_color")], Settings::ESP::enemyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("enemy_visible_color")], Settings::ESP::enemyVisibleColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("ally_color")], Settings::ESP::allyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("ally_visible_color")], Settings::ESP::allyVisibleColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("t_color")], Settings::ESP::tColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("t_visible_color")], Settings::ESP::tVisibleColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("ct_color")], Settings::ESP::ctColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("ct_visible_color")], Settings::ESP::ctVisibleColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("localplayer_color")], Settings::ESP::localplayerColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("bomb_color")], Settings::ESP::bombColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("bomb_defusing_color")], Settings::ESP::bombDefusingColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("hostage_color")], Settings::ESP::hostageColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("defuser_color")], Settings::ESP::defuserColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("weapon_color")], Settings::ESP::weaponColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("chicken_color")], Settings::ESP::chickenColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("fish_color")], Settings::ESP::fishColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("smoke_color")], Settings::ESP::smokeColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("decoy_color")], Settings::ESP::decoyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("flashbang_color")], Settings::ESP::flashbangColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("grenade_color")], Settings::ESP::grenadeColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("molotov_color")], Settings::ESP::molotovColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("mine_color")], Settings::ESP::mineColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("charge_color")], Settings::ESP::chargeColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("ally_info_color")], Settings::ESP::allyInfoColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("enemy_info_color")], Settings::ESP::enemyInfoColor);
    settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enabled")] = Settings::ESP::Glow::enabled;
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("ally_color")], Settings::ESP::Glow::allyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_color")], Settings::ESP::Glow::enemyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_visible_color")], Settings::ESP::Glow::enemyVisibleColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("localplayer_color")], Settings::ESP::Glow::localplayerColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("weapon_color")], Settings::ESP::Glow::weaponColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("grenade_color")], Settings::ESP::Glow::grenadeColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("defuser_color")], Settings::ESP::Glow::defuserColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("chicken_color")], Settings::ESP::Glow::chickenColor);
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("legit")] = Settings::ESP::Filters::legit;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("visibility_check")] = Settings::ESP::Filters::visibilityCheck;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("smoke_check")] = Settings::ESP::Filters::smokeCheck;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("enemies")] = Settings::ESP::Filters::enemies;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("allies")] = Settings::ESP::Filters::allies;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("bomb")] = Settings::ESP::Filters::bomb;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("hostages")] = Settings::ESP::Filters::hostages;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("defusers")] = Settings::ESP::Filters::defusers;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("weapons")] = Settings::ESP::Filters::weapons;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("chickens")] = Settings::ESP::Filters::chickens;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("fishes")] = Settings::ESP::Filters::fishes;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("throwables")] = Settings::ESP::Filters::throwables;
    settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("localplayer")] = Settings::ESP::Filters::localplayer;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("name")] = Settings::ESP::Info::name;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("clan")] = Settings::ESP::Info::clan;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("steam_id")] = Settings::ESP::Info::steamId;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rank")] = Settings::ESP::Info::rank;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("health")] = Settings::ESP::Info::health;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("armor")] = Settings::ESP::Info::armor;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("weapon")] = Settings::ESP::Info::weapon;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("scoped")] = Settings::ESP::Info::scoped;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("reloading")] = Settings::ESP::Info::reloading;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("flashed")] = Settings::ESP::Info::flashed;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("planting")] = Settings::ESP::Info::planting;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("has_defuser")] = Settings::ESP::Info::hasDefuser;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("defusing")] = Settings::ESP::Info::defusing;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("grabbing_hostage")] = Settings::ESP::Info::grabbingHostage;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rescuing")] = Settings::ESP::Info::rescuing;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("location")] = Settings::ESP::Info::location;
    settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("money")] = Settings::ESP::Info::money;
    
    // Visual Settings for Various Players
    // For Enemy
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Boxes")][XORSTR("enabled")] = Settings::ESP::FilterEnemy::Boxes::enabled;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Boxes")][XORSTR("type")] = (int)Settings::ESP::FilterEnemy::Boxes::type;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Skeleton")][XORSTR("enabled")] = Settings::ESP::FilterEnemy::Skeleton::enabled;
    LoadColor(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Skeleton")][XORSTR("ally_color")], Settings::ESP::FilterEnemy::Skeleton::allyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Skeleton")][XORSTR("enemy_color")], Settings::ESP::FilterEnemy::Skeleton::enemyColor);
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("HelthBar")][XORSTR("enabled")] = Settings::ESP::FilterEnemy::HelthBar::enabled;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("HelthBar")][XORSTR("type")] = (int)Settings::ESP::FilterEnemy::HelthBar::type;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Tracers")][XORSTR("enabled")] = Settings::ESP::FilterEnemy::Tracers::enabled;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Tracers")][XORSTR("type")] = (int)Settings::ESP::FilterEnemy::Tracers::type;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("BulletTracers")][XORSTR("enabled")] = Settings::ESP::FilterEnemy::BulletTracers::enabled;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("PlayerInfo")][XORSTR("enabled")] = Settings::ESP::FilterEnemy::playerInfo::enabled;
    
    // For LocalPlayer
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Boxes")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::Boxes::enabled;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Boxes")][XORSTR("type")] = (int)Settings::ESP::FilterLocalPlayer::Boxes::type;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Skeleton")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::Skeleton::enabled;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("FakeAngle")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::RealChams::enabled;

    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("FakeColor")], Settings::ESP::Chams::FakeColor);

    LoadColor(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Skeleton")][XORSTR("ally_color")], Settings::ESP::FilterLocalPlayer::Skeleton::allyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Skeleton")][XORSTR("enemy_color")], Settings::ESP::FilterLocalPlayer::Skeleton::enemyColor);
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Bars")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::HelthBar::enabled;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Bars")][XORSTR("color_type")] = (int)Settings::ESP::FilterLocalPlayer::HelthBar::colorType;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Bars")][XORSTR("type")] = (int)Settings::ESP::FilterLocalPlayer::HelthBar::type;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Tracers")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::Tracers::enabled;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Tracers")][XORSTR("type")] = (int)Settings::ESP::FilterLocalPlayer::Tracers::type;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("BulletTracers")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::BulletTracers::enabled;
    settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("PlayerInfo")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::playerInfo::enabled;
    
    // For Alise
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Boxes")][XORSTR("enabled")] = Settings::ESP::FilterAlise::Boxes::enabled;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Boxes")][XORSTR("type")] = (int)Settings::ESP::FilterAlise::Boxes::type;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Skeleton")][XORSTR("enabled")] = Settings::ESP::FilterAlise::Skeleton::enabled;
    LoadColor(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Skeleton")][XORSTR("ally_color")], Settings::ESP::FilterAlise::Skeleton::allyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Skeleton")][XORSTR("enemy_color")], Settings::ESP::FilterAlise::Skeleton::enemyColor);
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Bars")][XORSTR("enabled")] = Settings::ESP::FilterAlise::HelthBar::enabled;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Bars")][XORSTR("color_type")] = (int)Settings::ESP::FilterAlise::HelthBar::colorType;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Bars")][XORSTR("type")] = (int)Settings::ESP::FilterAlise::HelthBar::type;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Tracers")][XORSTR("enabled")] = Settings::ESP::FilterAlise::Tracers::enabled;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Tracers")][XORSTR("type")] = (int)Settings::ESP::FilterAlise::Tracers::type;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("BulletTracers")][XORSTR("enabled")] = Settings::ESP::FilterAlise::BulletTracers::enabled;
    settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("PlayerInfo")][XORSTR("enabled")] = Settings::ESP::FilterAlise::playerInfo::enabled;
    
    //Various Settings for Helth vae
    // For Enemy
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Bars")][XORSTR("enabled")] = &Settings::ESP::FilterEnemy::HelthBar::enabled;
    settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Bars")][XORSTR("type")] = (int*)&Settings::ESP::FilterEnemy::HelthBar::type;
    // END 


    settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("enabled")] = Settings::ESP::FOVCrosshair::enabled;
    settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("filled")] = Settings::ESP::FOVCrosshair::filled;
    LoadColor(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("color")], Settings::ESP::FOVCrosshair::color);

    // Visual Settings for diff players
    // For Enemy
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Enemy")][XORSTR("enabled")] = Settings::ESP::FilterEnemy::Chams::enabled;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Enemy")][XORSTR("type")] = (int)Settings::ESP::FilterEnemy::Chams::type;
    // For Local Player
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Localplayer")][XORSTR("enabled")] = Settings::ESP::FilterLocalPlayer::Chams::enabled;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Localplayer")][XORSTR("Fake")] = (int)Settings::ESP::FilterLocalPlayer::Chams::type;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Localplayer")][XORSTR("Real")] = (int)Settings::ESP::FilterLocalPlayer::RealChams::type;
    // For Alise
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Alise")][XORSTR("enabled")] = Settings::ESP::FilterAlise::Chams::enabled;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Alise")][XORSTR("type")] = (int)Settings::ESP::FilterAlise::Chams::type;
    
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Arms")][XORSTR("enabled")] = Settings::ESP::Chams::Arms::enabled;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Arms")][XORSTR("type")] = (int)Settings::ESP::Chams::Arms::type;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Weapon")][XORSTR("enabled")] = Settings::ESP::Chams::Weapon::enabled;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Weapon")][XORSTR("type")] = (int)Settings::ESP::Chams::Weapon::type;
    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Weapon")][XORSTR("color")], Settings::ESP::Chams::Weapon::color);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Arms")][XORSTR("color")], Settings::ESP::Chams::Arms::color);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_ally_color")], Settings::ESP::Chams::allyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_ally_visible_color")], Settings::ESP::Chams::allyVisibleColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_enemy_color")], Settings::ESP::Chams::enemyColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_enemy_visible_color")], Settings::ESP::Chams::enemyVisibleColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("localplayer_color")], Settings::ESP::Chams::localplayerColor);
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("type")] = (int)Settings::ESP::Chams::type;
    settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("enabled")] = Settings::ESP::Chams::enabled;
    settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("enabled")] = Settings::ESP::Sounds::enabled;
    settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("time")] = Settings::ESP::Sounds::time;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enabled")] = Settings::ESP::Hitmarker::enabled;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enemies")] = Settings::ESP::Hitmarker::enemies;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("allies")] = Settings::ESP::Hitmarker::allies;
    LoadColor(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("color")], Settings::ESP::Hitmarker::color);
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("duration")] = Settings::ESP::Hitmarker::duration;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("size")] = Settings::ESP::Hitmarker::size;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("inner_gap")] = Settings::ESP::Hitmarker::innerGap;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Damage")][XORSTR("enabled")] = Settings::ESP::Hitmarker::Damage::enabled;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("enabled")] = Settings::ESP::Hitmarker::Sounds::enabled;
    settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("sound")] = (int)Settings::ESP::Hitmarker::Sounds::sound;
    settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("enabled")] = Settings::ESP::HeadDot::enabled;
    settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("size")] = Settings::ESP::HeadDot::size;
    settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("enabled")] = Settings::ESP::Spread::enabled;
    settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("HitChance")] = Settings::ESP::Spread::spreadLimit;
    LoadColor(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("color")], Settings::ESP::Spread::color);
    LoadColor(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("spreadLimitColor")], Settings::ESP::Spread::spreadLimitColor);
    settings[XORSTR("ESP")][XORSTR("indicators")] = Settings::ESP::indicators;

    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDist")] = Settings::ESP::DangerZone::drawDist;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDistEnabled")] = Settings::ESP::DangerZone::drawDistEnabled;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade")] = Settings::ESP::DangerZone::upgrade;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate")] = Settings::ESP::DangerZone::lootcrate;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("radarjammer")] = Settings::ESP::DangerZone::radarjammer;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel")] = Settings::ESP::DangerZone::barrel;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox")] = Settings::ESP::DangerZone::ammobox;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe")] = Settings::ESP::DangerZone::safe;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun")] = Settings::ESP::DangerZone::dronegun;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone")] = Settings::ESP::DangerZone::drone;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash")] = Settings::ESP::DangerZone::cash;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet")] = Settings::ESP::DangerZone::tablet;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot")] = Settings::ESP::DangerZone::healthshot;
    settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee")] = Settings::ESP::DangerZone::melee;
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade_color")], Settings::ESP::DangerZone::upgradeColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate_color")], Settings::ESP::DangerZone::lootcrateColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("radarjammer_color")], Settings::ESP::DangerZone::radarjammerColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel_color")], Settings::ESP::DangerZone::barrelColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox_color")], Settings::ESP::DangerZone::ammoboxColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe_color")], Settings::ESP::DangerZone::safeColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun_color")], Settings::ESP::DangerZone::dronegunColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone_color")], Settings::ESP::DangerZone::droneColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash_color")], Settings::ESP::DangerZone::cashColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet_color")], Settings::ESP::DangerZone::tabletColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot_color")], Settings::ESP::DangerZone::healthshotColor);
    LoadColor(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee_color")], Settings::ESP::DangerZone::meleeColor);

    settings[XORSTR("Dlights")][XORSTR("enabled")] = Settings::Dlights::enabled;
    settings[XORSTR("Dlights")][XORSTR("radius")] = Settings::Dlights::radius;

    settings[XORSTR("TracerEffects")][XORSTR("enabled")] = Settings::TracerEffects::enabled;
    settings[XORSTR("TracerEffects")][XORSTR("serverSide")] = Settings::TracerEffects::serverSide;
    settings[XORSTR("TracerEffects")][XORSTR("effect")] = (int)Settings::TracerEffects::effect;
    settings[XORSTR("TracerEffects")][XORSTR("frequency")] = Settings::TracerEffects::frequency;

    settings[XORSTR("Spammer")][XORSTR("spammer_type")] = (int)Settings::Spammer::type;
    settings[XORSTR("Spammer")][XORSTR("say_team")] = Settings::Spammer::say_team;

    settings[XORSTR("Spammer")][XORSTR("KillSpammer")][XORSTR("enabled")] = Settings::Spammer::KillSpammer::enabled;
    settings[XORSTR("Spammer")][XORSTR("KillSpammer")][XORSTR("say_team")] = Settings::Spammer::KillSpammer::sayTeam;
    Json::Value killSpammerMessages;
    for (auto it : Settings::Spammer::KillSpammer::messages)
	killSpammerMessages.append(it);
    settings[XORSTR("Spammer")][XORSTR("KillSpammer")][XORSTR("messages")] = killSpammerMessages;

    Json::Value normalSpammerMessages;
    for (auto it : Settings::Spammer::NormalSpammer::messages)
	normalSpammerMessages.append(it);
    settings[XORSTR("Spammer")][XORSTR("NormalSpammer")][XORSTR("messages")] = normalSpammerMessages;

    settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_name")] = Settings::Spammer::PositionSpammer::showName;
    settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_weapon")] = Settings::Spammer::PositionSpammer::showWeapon;
    settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_rank")] = Settings::Spammer::PositionSpammer::showRank;
    settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_wins")] = Settings::Spammer::PositionSpammer::showWins;
    settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_health")] = Settings::Spammer::PositionSpammer::showHealth;
    settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_money")] = Settings::Spammer::PositionSpammer::showMoney;
    settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_lastplace")] = Settings::Spammer::PositionSpammer::showLastplace;

    settings[XORSTR("BHop")][XORSTR("enabled")] = Settings::BHop::enabled;
    settings[XORSTR("BHop")][XORSTR("Chance")][XORSTR("enabled")] = Settings::BHop::Chance::enabled;
    settings[XORSTR("BHop")][XORSTR("Chance")][XORSTR("value")] = Settings::BHop::Chance::value;
    settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("enabledMax")] = Settings::BHop::Hops::enabledMax;
    settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("Max")] = Settings::BHop::Hops::Max;
    settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("enabledMin")] = Settings::BHop::Hops::enabledMin;
    settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("Min")] = Settings::BHop::Hops::Min;

    settings[XORSTR("NoDuckCooldown")][XORSTR("enabled")] = Settings::NoDuckCooldown::enabled;

    settings[XORSTR("AutoStrafe")][XORSTR("enabled")] = Settings::AutoStrafe::enabled;
    settings[XORSTR("AutoStrafe")][XORSTR("type")] = (int)Settings::AutoStrafe::type;
    settings[XORSTR("AutoStrafe")][XORSTR("silent")] = Settings::AutoStrafe::silent;

    settings[XORSTR("Noflash")][XORSTR("enabled")] = Settings::Noflash::enabled;
    settings[XORSTR("Noflash")][XORSTR("value")] = Settings::Noflash::value;

    settings[XORSTR("Radar")][XORSTR("enabled")] = Settings::Radar::enabled;
    settings[XORSTR("Radar")][XORSTR("zoom")] = Settings::Radar::zoom;
    settings[XORSTR("Radar")][XORSTR("enemies")] = Settings::Radar::enemies;
    settings[XORSTR("Radar")][XORSTR("allies")] = Settings::Radar::allies;
    settings[XORSTR("Radar")][XORSTR("legit")] = Settings::Radar::legit;
    settings[XORSTR("Radar")][XORSTR("visibility_check")] = Settings::Radar::visibilityCheck;
    settings[XORSTR("Radar")][XORSTR("smoke_check")] = Settings::Radar::smokeCheck;
    settings[XORSTR("Radar")][XORSTR("InGame")][XORSTR("enabled")] = Settings::Radar::InGame::enabled;
    settings[XORSTR("Radar")][XORSTR("pos")][XORSTR("x")] = Settings::Radar::pos.x;
    settings[XORSTR("Radar")][XORSTR("pos")][XORSTR("y")] = Settings::Radar::pos.y;
    LoadColor(settings[XORSTR("Radar")][XORSTR("enemy_color")], Settings::Radar::enemyColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("enemy_visible_color")], Settings::Radar::enemyVisibleColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("ally_color")], Settings::Radar::allyColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("ally_visible_color")], Settings::Radar::allyVisibleColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("t_color")], Settings::Radar::tColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("t_visible_color")], Settings::Radar::tVisibleColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("ct_color")], Settings::Radar::ctColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("ct_visible_color")], Settings::Radar::ctVisibleColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("bomb_color")], Settings::Radar::bombColor);
    LoadColor(settings[XORSTR("Radar")][XORSTR("bomb_defusing_color")], Settings::Radar::bombDefusingColor);
    settings[XORSTR("Radar")][XORSTR("icons_scale")] = Settings::Radar::iconsScale;

    settings[XORSTR("Recoilcrosshair")][XORSTR("enabled")] = Settings::Recoilcrosshair::enabled;

    settings[XORSTR("Recoilcrosshair")][XORSTR("showOnlyWhenShooting")] = Settings::Recoilcrosshair::showOnlyWhenShooting;

    settings[XORSTR("FOVChanger")][XORSTR("enabled")] = Settings::FOVChanger::enabled;
    settings[XORSTR("FOVChanger")][XORSTR("value")] = Settings::FOVChanger::value;
    settings[XORSTR("FOVChanger")][XORSTR("viewmodel_enabled")] = Settings::FOVChanger::viewmodelEnabled;
    settings[XORSTR("FOVChanger")][XORSTR("viewmodel_value")] = Settings::FOVChanger::viewmodelValue;
    settings[XORSTR("FOVChanger")][XORSTR("ignore_scope")] = Settings::FOVChanger::ignoreScope;

    settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("enabled")] = Settings::Skinchanger::Skins::enabled;
    settings[XORSTR("SkinChanger")][XORSTR("Models")][XORSTR("enabled")] = Settings::Skinchanger::Models::enabled;
    settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("perTeam")] = Settings::Skinchanger::Skins::perTeam;

    for (const auto& item : Settings::Skinchanger::skinsCT)
    {
	const AttribItem_t& skin = item.second;

#define skinSetting settings[XORSTR("SkinChanger")][XORSTR("skinsCT")][Util::Items::GetItemConfigEntityName(item.first)]
	skinSetting[XORSTR("ItemDefinitionIndex")] = Util::Items::GetItemConfigEntityName(skin.itemDefinitionIndex);
	skinSetting[XORSTR("PaintKit")] = skin.fallbackPaintKit;
	skinSetting[XORSTR("Wear")] = skin.fallbackWear;
	skinSetting[XORSTR("Seed")] = skin.fallbackSeed;
	skinSetting[XORSTR("StatTrak")] = skin.fallbackStatTrak;
	skinSetting[XORSTR("CustomName")] = skin.customName;
#undef skinSetting
    }

    for (const auto& item : Settings::Skinchanger::skinsT)
    {
	const AttribItem_t& skin = item.second;

#define skinSetting settings[XORSTR("SkinChanger")][XORSTR("skinsT")][Util::Items::GetItemConfigEntityName(item.first)]
	skinSetting[XORSTR("ItemDefinitionIndex")] = Util::Items::GetItemConfigEntityName(skin.itemDefinitionIndex);
	skinSetting[XORSTR("PaintKit")] = skin.fallbackPaintKit;
	skinSetting[XORSTR("Wear")] = skin.fallbackWear;
	skinSetting[XORSTR("Seed")] = skin.fallbackSeed;
	skinSetting[XORSTR("StatTrak")] = skin.fallbackStatTrak;
	skinSetting[XORSTR("CustomName")] = skin.customName;
#undef skinSetting
    }

    settings[XORSTR("Legitbot")][XORSTR("enabled")] = Settings::Legitbot::enabled;
    settings[XORSTR("Ragebot")][XORSTR("enabled")] = Settings::Ragebot::enabled;
    settings[XORSTR("ShowRanks")][XORSTR("enabled")] = Settings::ShowRanks::enabled;

    settings[XORSTR("ShowSpectators")][XORSTR("enabled")] = Settings::ShowSpectators::enabled;

    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posX")] = Settings::UI::Windows::Colors::posX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posY")] = Settings::UI::Windows::Colors::posY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeX")] = Settings::UI::Windows::Colors::sizeX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeY")] = Settings::UI::Windows::Colors::sizeY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("open")] = Settings::UI::Windows::Colors::open;

    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posX")] = Settings::UI::Windows::Config::posX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posY")] = Settings::UI::Windows::Config::posY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeX")] = Settings::UI::Windows::Config::sizeX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeY")] = Settings::UI::Windows::Config::sizeY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("open")] = Settings::UI::Windows::Config::open;

    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posX")] = Settings::UI::Windows::Main::posX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posY")] = Settings::UI::Windows::Main::posY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeX")] = Settings::UI::Windows::Main::sizeX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeY")] = Settings::UI::Windows::Main::sizeY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("open")] = Settings::UI::Windows::Main::open;

    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("posX")] = Settings::UI::Windows::Playerlist::posX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("posY")] = Settings::UI::Windows::Playerlist::posY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("sizeX")] = Settings::UI::Windows::Playerlist::sizeX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("sizeY")] = Settings::UI::Windows::Playerlist::sizeY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("open")] = Settings::UI::Windows::Playerlist::open;

    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posX")] = Settings::UI::Windows::Skinmodel::posX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posY")] = Settings::UI::Windows::Skinmodel::posY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeX")] = Settings::UI::Windows::Skinmodel::sizeX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeY")] = Settings::UI::Windows::Skinmodel::sizeY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("open")] = Settings::UI::Windows::Skinmodel::open;

    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posX")] = Settings::UI::Windows::Spectators::posX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posY")] = Settings::UI::Windows::Spectators::posY;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeX")] = Settings::UI::Windows::Spectators::sizeX;
    settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeY")] = Settings::UI::Windows::Spectators::sizeY;

    settings[XORSTR("ClanTagChanger")][XORSTR("value")] = Settings::ClanTagChanger::value;
    settings[XORSTR("ClanTagChanger")][XORSTR("enabled")] = Settings::ClanTagChanger::enabled;
    settings[XORSTR("ClanTagChanger")][XORSTR("animation")] = Settings::ClanTagChanger::animation;
    settings[XORSTR("ClanTagChanger")][XORSTR("animation_speed")] = Settings::ClanTagChanger::animationSpeed;
    settings[XORSTR("ClanTagChanger")][XORSTR("type")] = (int)Settings::ClanTagChanger::type;

    settings[XORSTR("View")][XORSTR("NoViewPunch")][XORSTR("enabled")] = Settings::View::NoViewPunch::enabled;
    settings[XORSTR("View")][XORSTR("NoAimPunch")][XORSTR("enabled")] = Settings::View::NoAimPunch::enabled;
    settings[XORSTR("FakeWalk")][XORSTR("enabled")] = Settings::FakeWalk::enabled;

    settings[XORSTR("FakeLag")][XORSTR("enabled")] = Settings::FakeLag::enabled;
    settings[XORSTR("FakeLag")][XORSTR("value")] = Settings::FakeLag::value;
    settings[XORSTR("FakeLag")][XORSTR("adaptive")] = Settings::FakeLag::adaptive;
    settings[XORSTR("FakeLag")][XORSTR("randomLag")] = Settings::AntiAim::randomLag::enabled;

    settings[XORSTR("SlowWalk")][XORSTR("enabled")] = Settings::AntiAim::SlowWalk::enabled;
    settings[XORSTR("SlowWalk")][XORSTR("key")] = Settings::AntiAim::SlowWalk::key;

    settings[XORSTR("AutoAccept")][XORSTR("enabled")] = Settings::AutoAccept::enabled;

    settings[XORSTR("NoSky")][XORSTR("enabled")] = Settings::NoSky::enabled;
    LoadColor(settings[XORSTR("NoSky")][XORSTR("color")], Settings::NoSky::color);

    settings[XORSTR("SkyBox")][XORSTR("enabled")] = Settings::SkyBox::enabled;
    settings[XORSTR("SkyBox")][XORSTR("skyBoxNumber")] = Settings::SkyBox::skyBoxNumber;

    settings[XORSTR("ASUSWalls")][XORSTR("enabled")] = Settings::ASUSWalls::enabled;
    LoadColor(settings[XORSTR("ASUSWalls")][XORSTR("color")], Settings::ASUSWalls::color);

    settings[XORSTR("NoScopeBorder")][XORSTR("enabled")] = Settings::NoScopeBorder::enabled;

    settings[XORSTR("SniperCrosshair")][XORSTR("enabled")] = Settings::SniperCrosshair::enabled;

    settings[XORSTR("Autoblock")][XORSTR("enabled")] = Settings::Autoblock::enabled;
    settings[XORSTR("Autoblock")][XORSTR("key")] = Settings::Autoblock::key;

    settings[XORSTR("AutoDefuse")][XORSTR("enabled")] = Settings::AutoDefuse::enabled;
    settings[XORSTR("AutoDefuse")][XORSTR("silent")] = Settings::AutoDefuse::silent;

    settings[XORSTR("NoSmoke")][XORSTR("enabled")] = Settings::NoSmoke::enabled;
    settings[XORSTR("NoSmoke")][XORSTR("type")] = (int)Settings::NoSmoke::type;

    settings[XORSTR("ScreenshotCleaner")][XORSTR("enabled")] = Settings::ScreenshotCleaner::enabled;

    settings[XORSTR("EdgeJump")][XORSTR("enabled")] = Settings::EdgeJump::enabled;
    settings[XORSTR("EdgeJump")][XORSTR("key")] = Util::GetButtonName(Settings::EdgeJump::key);

    settings[XORSTR("NameStealer")][XORSTR("enabled")] = Settings::NameStealer::enabled;
    settings[XORSTR("NameStealer")][XORSTR("team")] = Settings::NameStealer::team;

    settings[XORSTR("Eventlog")][XORSTR("showEnemies")] = Settings::Eventlog::showEnemies;
    settings[XORSTR("Eventlog")][XORSTR("showTeammates")] = Settings::Eventlog::showTeammates;
    settings[XORSTR("Eventlog")][XORSTR("showLocalplayer")] = Settings::Eventlog::showLocalplayer;
    settings[XORSTR("Eventlog")][XORSTR("duration")] = Settings::Eventlog::duration;
    settings[XORSTR("Eventlog")][XORSTR("lines")] = Settings::Eventlog::lines;
    LoadColor(settings[XORSTR("Eventlog")][XORSTR("color")], Settings::Eventlog::color);

    settings[XORSTR("ThirdPerson")][XORSTR("enabled")] = Settings::ThirdPerson::enabled;
    settings[XORSTR("ThirdPerson")][XORSTR("toggled")] = Settings::ThirdPerson::toggled;
    settings[XORSTR("ThirdPerson")][XORSTR("distance")] = Settings::ThirdPerson::distance;
    settings[XORSTR("ThirdPerson")][XORSTR("togglekey")] = Util::GetButtonName(Settings::ThirdPerson::toggleThirdPerson);

    settings[XORSTR("JumpThrow")][XORSTR("enabled")] = Settings::JumpThrow::enabled;
    settings[XORSTR("JumpThrow")][XORSTR("key")] = Util::GetButtonName(Settings::JumpThrow::key);

    settings[XORSTR("DisablePostProcessing")][XORSTR("enabled")] = Settings::DisablePostProcessing::enabled;
    settings[XORSTR("NoFall")][XORSTR("enabled")] = Settings::NoFall::enabled;
    settings[XORSTR("RagdollGravity")][XORSTR("enable")] = Settings::RagdollGravity::enabled;

    settings[XORSTR("GrenadeHelper")][XORSTR("enabled")] = Settings::GrenadeHelper::enabled;
    settings[XORSTR("GrenadeHelper")][XORSTR("aimAssist")] = Settings::GrenadeHelper::aimAssist;
    settings[XORSTR("GrenadeHelper")][XORSTR("OnlyMatching")] = Settings::GrenadeHelper::onlyMatchingInfos;
    settings[XORSTR("GrenadeHelper")][XORSTR("aimStep")] = Settings::GrenadeHelper::aimStep;
    settings[XORSTR("GrenadeHelper")][XORSTR("aimDistance")] = Settings::GrenadeHelper::aimDistance;
    settings[XORSTR("GrenadeHelper")][XORSTR("aimFov")] = Settings::GrenadeHelper::aimFov;
    LoadColor(settings[XORSTR("GrenadeHelper")][XORSTR("aimDot")], Settings::GrenadeHelper::aimDot);
    LoadColor(settings[XORSTR("GrenadeHelper")][XORSTR("aimLine")], Settings::GrenadeHelper::aimLine);
    LoadColor(settings[XORSTR("GrenadeHelper")][XORSTR("infoHe")], Settings::GrenadeHelper::infoHE);
    LoadColor(settings[XORSTR("GrenadeHelper")][XORSTR("infoSmoke")], Settings::GrenadeHelper::infoSmoke);
    LoadColor(settings[XORSTR("GrenadeHelper")][XORSTR("infoMolotov")], Settings::GrenadeHelper::infoMolotov);
    LoadColor(settings[XORSTR("GrenadeHelper")][XORSTR("infoFlash")], Settings::GrenadeHelper::infoFlash);

    settings[XORSTR("GrenadePrediction")][XORSTR("enabled")] = Settings::GrenadePrediction::enabled;
    LoadColor(settings[XORSTR("GrenadePrediction")][XORSTR("color")], Settings::GrenadePrediction::color);

    settings[XORSTR("AutoKnife")][XORSTR("enabled")] = Settings::AutoKnife::enabled;
    settings[XORSTR("AutoKnife")][XORSTR("Filters")][XORSTR("enemies")] = Settings::AutoKnife::Filters::enemies;
    settings[XORSTR("AutoKnife")][XORSTR("Filters")][XORSTR("allies")] = Settings::AutoKnife::Filters::allies;
    settings[XORSTR("AutoKnife")][XORSTR("onKey")] = Settings::AutoKnife::onKey;
    settings[XORSTR("QuickSwitch")][XORSTR("enabled")] = Settings::QuickSwitch::enabled;
    std::ofstream(path) << styledWriter.write(settings);
}

void Settings::LoadConfig(std::string path)
{
    TracerEffect::RestoreTracers();
    if (!std::ifstream(path).good())
    {
	Settings::LoadDefaultsOrSave(path);
	return;
    }

    Json::Value settings;
    std::ifstream configDoc(path, std::ifstream::binary);
    configDoc >> settings;

    GetVal(settings[XORSTR("UI")][XORSTR("mainColor")], &Settings::UI::mainColor);
    GetVal(settings[XORSTR("UI")][XORSTR("bodyColor")], &Settings::UI::bodyColor);
    GetVal(settings[XORSTR("UI")][XORSTR("fontColor")], &Settings::UI::fontColor);
    GetVal(settings[XORSTR("UI")][XORSTR("accentColor")], &Settings::UI::accentColor);
    GetVal(settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("family")], &Settings::UI::Fonts::ESP::family);
    GetVal(settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("size")], &Settings::UI::Fonts::ESP::size);
    GetVal(settings[XORSTR("UI")][XORSTR("Fonts")][XORSTR("ESP")][XORSTR("flags")], &Settings::UI::Fonts::ESP::flags);

    Fonts::SetupFonts();

    Settings::Legitbot::weapons = {
	{ ItemDefinitionIndex::INVALID, defaultSettings },
    };

    for (Json::ValueIterator itr = settings[XORSTR("Legitbot")][XORSTR("weapons")].begin(); itr != settings[XORSTR("Legitbot")][XORSTR("weapons")].end(); itr++)
    {
	    std::string weaponDataKey = itr.key().asString();
	    auto LegitweaponSetting = settings[XORSTR("Legitbot")][XORSTR("weapons")][weaponDataKey];

	    // XXX Using exception handling to deal with this is stupid, but I don't care to find a better solution
	    // XXX We can't use GetOrdinal() since the key type is a string...
	    ItemDefinitionIndex weaponID;
	    try
	    {
	        weaponID = (ItemDefinitionIndex)std::stoi(weaponDataKey);
	    }
	    catch (std::invalid_argument&) // Not a number
	    {
	        weaponID = Util::Items::GetItemIndex(weaponDataKey);
	    }

	    if (Settings::Legitbot::weapons.find(weaponID) == Settings::Legitbot::weapons.end())
	        Settings::Legitbot::weapons[weaponID] = LegitWeapon_t();

	    LegitWeapon_t weapon = {
	        .silent = LegitweaponSetting[XORSTR("Silent")].asBool(),
	        .autoShoot = LegitweaponSetting[XORSTR("AutoShoot")].asBool(),
	        .aimkeyOnly = LegitweaponSetting[XORSTR("AimKeyOnly")].asBool(),
	        .smoothEnabled = LegitweaponSetting[XORSTR("Smooth")][XORSTR("Enabled")].asBool(),
	        .smoothSaltEnabled = LegitweaponSetting[XORSTR("Smooth")][XORSTR("Salting")][XORSTR("Enabled")].asBool(),
	        .errorMarginEnabled = LegitweaponSetting[XORSTR("ErrorMargin")][XORSTR("Enabled")].asBool(),
	        .autoAimEnabled = LegitweaponSetting[XORSTR("AutoAim")][XORSTR("Enabled")].asBool(),
	        .aimStepEnabled = LegitweaponSetting[XORSTR("AimStep")][XORSTR("Enabled")].asBool(),
	        .rcsEnabled = LegitweaponSetting[XORSTR("RCS")][XORSTR("Enabled")].asBool(),
	        .rcsAlwaysOn = LegitweaponSetting[XORSTR("RCS")][XORSTR("AlwaysOn")].asBool(),
	        .hitchanceEnaled = LegitweaponSetting[XORSTR("HitChance")][XORSTR("Enabled")].asBool(),
	        .autoPistolEnabled = LegitweaponSetting[XORSTR("AutoPistol")][XORSTR("Enabled")].asBool(),
	        .autoScopeEnabled = LegitweaponSetting[XORSTR("AutoScope")][XORSTR("Enabled")].asBool(),
	        .ignoreJumpEnabled = LegitweaponSetting[XORSTR("IgnoreJump")][XORSTR("Enabled")].asBool(),
	        .ignoreEnemyJumpEnabled = LegitweaponSetting[XORSTR("IgnoreEnemyJump")][XORSTR("Enabled")].asBool(),
	        .autoSlow = LegitweaponSetting[XORSTR("AutoSlow")][XORSTR("enabled")].asBool(),
	        .predEnabled = LegitweaponSetting[XORSTR("Prediction")][XORSTR("enabled")].asBool(),
            .TriggerBot = LegitweaponSetting[XORSTR("TriggerBot")][XORSTR("enabled")].asBool(),
            .mindamage = LegitweaponSetting[XORSTR("MinDamage")][XORSTR("enabled")].asBool(),
            .autoWall = LegitweaponSetting[XORSTR("Autowall")][XORSTR("enabled")].asBool(),
	        .bone = LegitweaponSetting[XORSTR("TargetBone")].asInt(),
	        .smoothType = (SmoothType)LegitweaponSetting[XORSTR("Smooth")][XORSTR("Type")].asInt(),
	        .aimkey = Util::GetButtonCode(LegitweaponSetting[XORSTR("AimKey")].asCString()),
	        .smoothAmount = LegitweaponSetting[XORSTR("Smooth")][XORSTR("Amount")].asFloat(),
	        .smoothSaltMultiplier = LegitweaponSetting[XORSTR("Smooth")][XORSTR("Salting")][XORSTR("Multiplier")].asFloat(),
	        .errorMarginValue = LegitweaponSetting[XORSTR("ErrorMargin")][XORSTR("Value")].asFloat(),
	        .LegitautoAimFov = LegitweaponSetting[XORSTR("AutoAim")][XORSTR("LegitFOV")].asFloat(),
	        .aimStepMin = LegitweaponSetting[XORSTR("AimStep")][XORSTR("min")].asFloat(),
	        .aimStepMax = LegitweaponSetting[XORSTR("AimStep")][XORSTR("max")].asFloat(),
	        .rcsAmountX = LegitweaponSetting[XORSTR("RCS")][XORSTR("AmountX")].asFloat(),
	        .rcsAmountY = LegitweaponSetting[XORSTR("RCS")][XORSTR("AmountY")].asFloat(),
	        .minDamagevalue = LegitweaponSetting[XORSTR("MinDamage")][XORSTR("Value")].asFloat(),
	        .hitchance = LegitweaponSetting[XORSTR("HitChance")][XORSTR("Value")].asFloat(),
	    };

	    for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
	        weapon.desiredBones[bone] = LegitweaponSetting[XORSTR("DesiredBones")][XORSTR("Bones")][bone].asBool();
	    Settings::Legitbot::weapons.at(weaponID) = weapon;
    }

    Settings::Ragebot::weapons = {
	    { ItemDefinitionIndex::INVALID, ragedefault },
    };

    for (Json::ValueIterator itr = settings[XORSTR("Ragebot")][XORSTR("weapons")].begin(); itr != settings[XORSTR("Ragebot")][XORSTR("weapons")].end(); itr++)
    {
	    std::string RageweaponDataKey = itr.key().asString();
	    auto RageweaponSetting = settings[XORSTR("Ragebot")][XORSTR("weapons")][RageweaponDataKey];

	    // XXX Using exception handling to deal with this is stupid, but I don't care to find a better solution
	    // XXX We can't use GetOrdinal() since the key type is a string...
	    ItemDefinitionIndex weaponID;
	    try
	    {
	        weaponID = (ItemDefinitionIndex)std::stoi(RageweaponDataKey);
	    }
	    catch (std::invalid_argument&) // Not a number
	    {
	        weaponID = Util::Items::GetItemIndex(RageweaponDataKey);
	    }

	    // loading ragebot default
	    if (Settings::Ragebot::weapons.find(weaponID) == Settings::Ragebot::weapons.end())
	        Settings::Ragebot::weapons[weaponID] = RageWeapon_t();

	    RageWeapon_t weapon = {
	        .silent = RageweaponSetting[XORSTR("Silent")].asBool(),
	        .friendly = RageweaponSetting[XORSTR("Friendly")].asBool(),
	        .closestBone = RageweaponSetting[XORSTR("ClosestBone")].asBool(),
	        .HitChanceEnabled = RageweaponSetting[XORSTR("HitChance")][XORSTR("Enabled")].asBool(),
	        .autoPistolEnabled = RageweaponSetting[XORSTR("AutoPistol")][XORSTR("Enabled")].asBool(),
	        .autoShootEnabled = RageweaponSetting[XORSTR("AutoShoot")][XORSTR("Enabled")].asBool(),
	        .autoScopeEnabled = RageweaponSetting[XORSTR("AutoScope")][XORSTR("Enabled")].asBool(),
	        .autoSlow = RageweaponSetting[XORSTR("AutoSlow")][XORSTR("Enabled")].asBool(),
	        .scopeControlEnabled = RageweaponSetting[XORSTR("ScopeControl")][XORSTR("Enabled")].asBool(),
		    .DoubleFire = RageweaponSetting[XORSTR("Prediction")][XORSTR("Enabled")].asBool(),
            .MinDamage = RageweaponSetting[XORSTR("MinDamage")].asFloat(),
	        .HitChance = RageweaponSetting[XORSTR("HitChance")][XORSTR("Value")].asFloat(),
        };
        // Getting value like this because can't find anyother way convert value from json to enum
        GetVal(RageweaponSetting[XORSTR("DamagePrediction")][XORSTR("Type")], (int*)&weapon.DmagePredictionType);
        GetVal(RageweaponSetting[XORSTR("EnemySelectionType")][XORSTR("Type")], (int*)&weapon.enemySelectionType);

	    for (int bone = 0; bone < 6; bone++)
        {
            weapon.desireBones[bone] = RageweaponSetting[XORSTR("DesireBones")][XORSTR("Bones")][bone].asBool();
            weapon.desiredMultiBones[bone] = RageweaponSetting[XORSTR("DesireMultiBones")][XORSTR("Bones")][bone].asBool();
        } 
        Settings::Ragebot::weapons.at(weaponID) = weapon;
    }

    GetVal(settings[XORSTR("Legitbot")][XORSTR("AutoCrouch")][XORSTR("enabled")], &Settings::Legitbot::AutoCrouch::enabled);
    GetVal(settings[XORSTR("Legitbot")][XORSTR("enabled")], &Settings::Legitbot::enabled);
    GetVal(settings[XORSTR("Ragebot")][XORSTR("enabled")], &Settings::Ragebot::enabled);
    GetVal(settings[XORSTR("Ragebot")][XORSTR("AUtoAcrouth")][XORSTR("enable")], &Settings::Ragebot::AutoCrouch::enable);
    GetVal(settings[XORSTR("Ragebot")][XORSTR("BackTrack")][XORSTR("enable")], &Settings::Ragebot::backTrack::enabled);
    GetVal(settings[XORSTR("Ragebot")][XORSTR("BackTrack")][XORSTR("time")], &Settings::Ragebot::backTrack::time);

    GetVal(settings[XORSTR("Triggerbot")][XORSTR("enabled")], &Settings::Triggerbot::enabled);
	GetButtonCode(settings[XORSTR("Triggerbot")][XORSTR("key")], &Settings::Triggerbot::key);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("enemies")], &Settings::Triggerbot::Filters::enemies);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("allies")], &Settings::Triggerbot::Filters::allies);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("walls")], &Settings::Triggerbot::Filters::walls);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("smoke_check")], &Settings::Triggerbot::Filters::smokeCheck);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("flash_check")], &Settings::Triggerbot::Filters::flashCheck);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("head")], &Settings::Triggerbot::Filters::head);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("chest")], &Settings::Triggerbot::Filters::chest);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("stomach")], &Settings::Triggerbot::Filters::stomach);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("arms")], &Settings::Triggerbot::Filters::arms);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("Filters")][XORSTR("legs")], &Settings::Triggerbot::Filters::legs);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("RandomDelay")][XORSTR("enabled")], &Settings::Triggerbot::RandomDelay::enabled);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("RandomDelay")][XORSTR("lowBound")], &Settings::Triggerbot::RandomDelay::lowBound);
	GetVal(settings[XORSTR("Triggerbot")][XORSTR("RandomDelay")][XORSTR("highBound")], &Settings::Triggerbot::RandomDelay::highBound);
    
    GetVal( settings[XORSTR("AntiAim")][XORSTR("Type")], (int*)&Settings::AntiAim::Type::antiaimType );
    GetVal( settings[XORSTR("AntiAim")][XORSTR("Yaw")][XORSTR("type")], (int*)&Settings::AntiAim::Yaw::typeReal);
	GetVal(settings[XORSTR("AntiAim")][XORSTR("Pitchtype")], (int*)&Settings::AntiAim::pitchtype);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("AutoDisable")][XORSTR("no_enemy")], &Settings::AntiAim::AutoDisable::noEnemy);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("AutoDisable")][XORSTR("knife_held")], &Settings::AntiAim::AutoDisable::knifeHeld);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("FakeDuck")][XORSTR("enabled")], &Settings::AntiAim::FakeDuck::enabled);
        GetButtonCode(settings[XORSTR("AntiAim")][XORSTR("FakeDuck")][XORSTR("fakeDuckKey")], &Settings::AntiAim::FakeDuck::fakeDuckKey);
    	GetVal(settings[XORSTR("Ragebot")][XORSTR("quickpeek")][XORSTR("enabled")], &Settings::Ragebot::quickpeek::enabled);
        GetButtonCode(settings[XORSTR("Ragebot")][XORSTR("quickpeek")][XORSTR("key")], &Settings::Ragebot::quickpeek::key);

        GetVal(settings[XORSTR("SilentWalk")][XORSTR("enabled")], &Settings::SilentWalk::enabled);
        GetButtonCode(settings[XORSTR("SilentWalk")][XORSTR("Key")], &Settings::SilentWalk::key);

    // Settings for RageAntiAIm
    GetVal(settings[XORSTR("AntiAim")][XORSTR("airspin")][XORSTR("enabled")], &Settings::AntiAim::airspin::enabled);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("enabled")], &Settings::AntiAim::RageAntiAim::enable);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("invertonhurt")], &Settings::AntiAim::RageAntiAim::invertOnHurt);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("Fake Percent")], &Settings::AntiAim::RageAntiAim::AntiAImPercent);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("Real Jitter Percent")], &Settings::AntiAim::RageAntiAim::JitterPercent);
    GetButtonCode(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("togglekey")], &Settings::AntiAim::RageAntiAim::InvertKey);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("inverted")], &Settings::AntiAim::RageAntiAim::inverted);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("Type")], (int*)&Settings::AntiAim::RageAntiAim::Type);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("LbyMode")], (int*)&Settings::AntiAim::RageAntiAim::lbym);

    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("atTheTarget")], &Settings::AntiAim::RageAntiAim::atTheTarget);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("SendReal")], &Settings::AntiAim::RageAntiAim::SendReal);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("Enable")], &Settings::AntiAim::ManualAntiAim::Enable);
    GetButtonCode(settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("backButton")], &Settings::AntiAim::ManualAntiAim::backButton);
    GetButtonCode(settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("RightButton")], &Settings::AntiAim::ManualAntiAim::RightButton);
    GetButtonCode(settings[XORSTR("AntiAim")][XORSTR("ManualAntiAim")][XORSTR("LeftButton")], &Settings::AntiAim::ManualAntiAim::LeftButton);


    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("Offset")], &Settings::AntiAim::RageAntiAim::offset);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Rage AntiAim")][XORSTR("pitchJitter")], &Settings::AntiAim::RageAntiAim::pitchJitter);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("LBYJitter")], &Settings::AntiAim::lbyjitter);

    GetVal(settings[XORSTR("SvCheats")][XORSTR("enabled")], &Settings::SvCheats::enabled);
    GetVal(settings[XORSTR("SvCheatss")][XORSTR("enabled")], &Settings::SvCheats::svcheats::enabled);

    GetVal(settings[XORSTR("aspect")][XORSTR("enabled")], &Settings::SvCheats::aspect::enabled);
    GetVal(settings[XORSTR("aspect")][XORSTR("var")], &Settings::SvCheats::aspect::var);

    GetVal(settings[XORSTR("bright")][XORSTR("enabled")], &Settings::SvCheats::bright::enabled);
    GetVal(settings[XORSTR("fog")][XORSTR("enabled")], &Settings::SvCheats::fog::enabled);
    GetVal(settings[XORSTR("impacts")][XORSTR("enabled")], &Settings::SvCheats::impacts::enabled);
    GetVal(settings[XORSTR("grenadetraj")][XORSTR("enabled")], &Settings::SvCheats::grenadetraj::enabled);


    GetVal(settings[XORSTR("gravity")][XORSTR("enabled")], &Settings::SvCheats::gravity::enabled);

    GetVal(settings[XORSTR("viewmodel")][XORSTR("enabled")], &Settings::SvCheats::viewmodel::enabled);
    GetVal(settings[XORSTR("viewmodelfov")][XORSTR("fov")], &Settings::SvCheats::viewmodel::fov);

    GetVal(settings[XORSTR("gravity")][XORSTR("amount")], &Settings::SvCheats::gravity::amount);

    GetVal(settings[XORSTR("viewmodelx")][XORSTR("x")], &Settings::SvCheats::viewmodel::x);
    GetVal(settings[XORSTR("viewmodely")][XORSTR("y")], &Settings::SvCheats::viewmodel::y);
    GetVal(settings[XORSTR("viewmodelz")][XORSTR("z")], &Settings::SvCheats::viewmodel::z);

    GetVal(settings[XORSTR("BuyBot")][XORSTR("enabled")], &Settings::buybot::enabled);
    GetVal(settings[XORSTR("BuyBot")][XORSTR("scout")], &Settings::buybot::scout);
    GetVal(settings[XORSTR("BuyBot")][XORSTR("autosniper")], &Settings::buybot::autosniper);


    /* 
    *Legit Anti AIm Settings Saving
    */
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("enabled")], &Settings::AntiAim::LegitAntiAim::enable);
    GetButtonCode(settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("togglekey")], &Settings::AntiAim::LegitAntiAim::InvertKey);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("inverted")], &Settings::AntiAim::LegitAntiAim::inverted);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("RealPercentage")], &Settings::AntiAim::LegitAntiAim::RealPercentage);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("RealPercentageInCroutch")], &Settings::AntiAim::LegitAntiAim::RealPercentageInCroutch);
    // GetVal(settings[XORSTR("AntiAim")][XORSTR("Legit AntiAim")][XORSTR("OverWatchProof")], (int*)&Settings::AntiAim::LegitAntiAim::legitAAtype);
    // End legit Anti aim settings Features

    GetVal(settings[XORSTR("AntiAim")][XORSTR("HeadEdge")][XORSTR("enabled")], &Settings::AntiAim::HeadEdge::enabled);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("HeadEdge")][XORSTR("distance")], &Settings::AntiAim::HeadEdge::distance);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("LBYBreaker")][XORSTR("enabled")], &Settings::AntiAim::LBYBreaker::enabled);
    GetVal(settings[XORSTR("AntiAim")][XORSTR("LBYBreaker")][XORSTR("offset")], &Settings::AntiAim::LBYBreaker::offset);
    GetVal(settings[XORSTR("Resolver")][XORSTR("resolve_allAP")], &Settings::Resolver::resolveAllAP);
    GetVal(settings[XORSTR("Resolver")][XORSTR("rPitch")], &Settings::Resolver::rPitch);

    GetVal(settings[XORSTR("Resolver")][XORSTR("resolve_all")], &Settings::Resolver::resolveAll);
    GetVal(settings[XORSTR("Resolver")][XORSTR("manual")], &Settings::Resolver::manual);
    GetVal(settings[XORSTR("Resolver")][XORSTR("goalFeetYaw")], &Settings::Resolver::goalFeetYaw);
    GetVal(settings[XORSTR("Resolver")][XORSTR("EyeAngles")], &Settings::Resolver::EyeAngles);
    GetVal( settings[XORSTR("Resolver")][XORSTR("Type")], (int*)&Settings::Resolver::resolverType);

    GetVal( settings[XORSTR("Ragebot")][XORSTR("impactType")], (int*)&Settings::Ragebot::impacttype);

    GetVal(settings[XORSTR("Ragebot")][XORSTR("quickpeek")][XORSTR("color")], &Settings::Ragebot::quickpeek::color);
    GetVal(settings[XORSTR("ESP")][XORSTR("Watermark")][XORSTR("color")], &Settings::ESP::Watermark::color);
    GetVal(settings[XORSTR("ESP")][XORSTR("manualAAColor")], &Settings::ESP::manualAAColor);
    GetVal(settings[XORSTR("UI")][XORSTR("particles")], &Settings::UI::particles);

    GetVal(settings[XORSTR("ESP")][XORSTR("KeybindsY")], &Settings::ESP::keybi::y);
    GetVal(settings[XORSTR("ESP")][XORSTR("KeybindsX")], &Settings::ESP::keybi::x);
    GetVal(settings[XORSTR("ESP")][XORSTR("showKeybinds")], &Settings::ESP::KeyBinds);
    GetVal(settings[XORSTR("ESP")][XORSTR("showDormant")], &Settings::ESP::showDormant);
    GetVal(settings[XORSTR("ESP")][XORSTR("showimpacts")], &Settings::ESP::showimpacts);
    GetVal(settings[XORSTR("ESP")][XORSTR("enabled")], &Settings::ESP::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("backend")], (int*)&Settings::ESP::backend);
    GetButtonCode(settings[XORSTR("ESP")][XORSTR("key")], &Settings::ESP::key);
    GetVal(settings[XORSTR("ESP")][XORSTR("enemy_color")], &Settings::ESP::enemyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("enemy_visible_color")], &Settings::ESP::enemyVisibleColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("ally_color")], &Settings::ESP::allyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("ally_visible_color")], &Settings::ESP::allyVisibleColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("t_color")], &Settings::ESP::tColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("t_visible_color")], &Settings::ESP::tVisibleColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("ct_color")], &Settings::ESP::ctColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("ct_visible_color")], &Settings::ESP::ctVisibleColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("localplayer_color")], &Settings::ESP::localplayerColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("bomb_color")], &Settings::ESP::bombColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("bomb_defusing_color")], &Settings::ESP::bombDefusingColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("hostage_color")], &Settings::ESP::hostageColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("defuser_color")], &Settings::ESP::defuserColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("weapon_color")], &Settings::ESP::weaponColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("chicken_color")], &Settings::ESP::chickenColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("fish_color")], &Settings::ESP::fishColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("smoke_color")], &Settings::ESP::smokeColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("decoy_color")], &Settings::ESP::decoyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("flashbang_color")], &Settings::ESP::flashbangColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("grenade_color")], &Settings::ESP::grenadeColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("molotov_color")], &Settings::ESP::molotovColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("mine_color")], &Settings::ESP::mineColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("charge_color")], &Settings::ESP::chargeColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("ally_info_color")], &Settings::ESP::allyInfoColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("enemy_info_color")], &Settings::ESP::enemyInfoColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enabled")], &Settings::ESP::Glow::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("ally_color")], &Settings::ESP::Glow::allyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_color")], &Settings::ESP::Glow::enemyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("enemy_visible_color")], &Settings::ESP::Glow::enemyVisibleColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("localplayer_color")], &Settings::ESP::Glow::localplayerColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("weapon_color")], &Settings::ESP::Glow::weaponColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("grenade_color")], &Settings::ESP::Glow::grenadeColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("defuser_color")], &Settings::ESP::Glow::defuserColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Glow")][XORSTR("chicken_color")], &Settings::ESP::Glow::chickenColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("legit")], &Settings::ESP::Filters::legit);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("visibility_check")], &Settings::ESP::Filters::visibilityCheck);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("smoke_check")], &Settings::ESP::Filters::smokeCheck);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("enemies")], &Settings::ESP::Filters::enemies);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("allies")], &Settings::ESP::Filters::allies);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("bomb")], &Settings::ESP::Filters::bomb);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("hostages")], &Settings::ESP::Filters::hostages);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("defusers")], &Settings::ESP::Filters::defusers);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("weapons")], &Settings::ESP::Filters::weapons);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("chickens")], &Settings::ESP::Filters::chickens);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("fishes")], &Settings::ESP::Filters::fishes);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("throwables")], &Settings::ESP::Filters::throwables);
    GetVal(settings[XORSTR("ESP")][XORSTR("Filters")][XORSTR("localplayer")], &Settings::ESP::Filters::localplayer);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("name")], &Settings::ESP::Info::name);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("clan")], &Settings::ESP::Info::clan);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("steam_id")], &Settings::ESP::Info::steamId);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rank")], &Settings::ESP::Info::rank);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("health")], &Settings::ESP::Info::health);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("armor")], &Settings::ESP::Info::armor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("weapon")], &Settings::ESP::Info::weapon);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("scoped")], &Settings::ESP::Info::scoped);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("reloading")], &Settings::ESP::Info::reloading);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("flashed")], &Settings::ESP::Info::flashed);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("planting")], &Settings::ESP::Info::planting);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("has_defuser")], &Settings::ESP::Info::hasDefuser);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("defusing")], &Settings::ESP::Info::defusing);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("grabbing_hostage")], &Settings::ESP::Info::grabbingHostage);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("rescuing")], &Settings::ESP::Info::rescuing);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("location")], &Settings::ESP::Info::location);
    GetVal(settings[XORSTR("ESP")][XORSTR("Info")][XORSTR("money")], &Settings::ESP::Info::money);
    
    // Visual Settings for various players
    // For Enemy
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Boxes")][XORSTR("enabled")], &Settings::ESP::FilterEnemy::Boxes::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Boxes")][XORSTR("type")], (int*)&Settings::ESP::FilterEnemy::Boxes::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Skeleton")][XORSTR("enabled")], &Settings::ESP::FilterEnemy::Skeleton::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Skeleton")][XORSTR("ally_color")], &Settings::ESP::FilterEnemy::Skeleton::allyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Skeleton")][XORSTR("enemy_color")], &Settings::ESP::FilterEnemy::Skeleton::enemyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("HelthBar")][XORSTR("enabled")], &Settings::ESP::FilterEnemy::HelthBar::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("HelthBar")][XORSTR("type")], (int*)&Settings::ESP::FilterEnemy::HelthBar::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Tracers")][XORSTR("enabled")], &Settings::ESP::FilterEnemy::Tracers::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("Tracers")][XORSTR("type")], (int*)&Settings::ESP::FilterEnemy::Tracers::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("BulletTracers")][XORSTR("enabled")], &Settings::ESP::FilterEnemy::BulletTracers::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Enemy")][XORSTR("PlayerInfo")][XORSTR("enabled")], &Settings::ESP::FilterEnemy::playerInfo::enabled);
    // For LocalPlayer
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("FakeColor")], &Settings::ESP::Chams::FakeColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Boxes")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::Boxes::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Boxes")][XORSTR("type")], (int*)&Settings::ESP::FilterLocalPlayer::Boxes::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Skeleton")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::Skeleton::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("FakeAngle")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::RealChams::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Skeleton")][XORSTR("ally_color")], &Settings::ESP::FilterLocalPlayer::Skeleton::allyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Skeleton")][XORSTR("enemy_color")], &Settings::ESP::FilterLocalPlayer::Skeleton::enemyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("HelthBar")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::HelthBar::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("HelthBar")][XORSTR("type")], (int*)&Settings::ESP::FilterLocalPlayer::HelthBar::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Tracers")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::Tracers::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("Tracers")][XORSTR("type")], (int*)&Settings::ESP::FilterLocalPlayer::Tracers::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("BulletTracers")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::BulletTracers::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Localplayer")][XORSTR("PlayerInfo")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::playerInfo::enabled);
    //For Alise
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Boxes")][XORSTR("enabled")], &Settings::ESP::FilterAlise::Boxes::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Boxes")][XORSTR("type")], (int*)&Settings::ESP::FilterAlise::Boxes::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Skeleton")][XORSTR("enabled")], &Settings::ESP::FilterAlise::Skeleton::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Skeleton")][XORSTR("ally_color")], &Settings::ESP::FilterAlise::Skeleton::allyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Skeleton")][XORSTR("enemy_color")], &Settings::ESP::FilterAlise::Skeleton::enemyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("HelthBar")][XORSTR("enabled")], &Settings::ESP::FilterAlise::HelthBar::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("HelthBar")][XORSTR("type")], (int*)&Settings::ESP::FilterAlise::HelthBar::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Tracers")][XORSTR("enabled")], &Settings::ESP::FilterAlise::Tracers::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("Tracers")][XORSTR("type")], (int*)&Settings::ESP::FilterAlise::Tracers::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("BulletTracers")][XORSTR("enabled")], &Settings::ESP::FilterAlise::BulletTracers::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Alise")][XORSTR("PlayerInfo")][XORSTR("enabled")], &Settings::ESP::FilterAlise::playerInfo::enabled);
    // END

    GetVal(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("enabled")], &Settings::ESP::FOVCrosshair::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("filled")], &Settings::ESP::FOVCrosshair::filled);
    GetVal(settings[XORSTR("ESP")][XORSTR("FOVCrosshair")][XORSTR("color")], &Settings::ESP::FOVCrosshair::color);

    // Visual Settigs For Diff Player
    // For Enemy
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Enemy")][XORSTR("enabled")], &Settings::ESP::FilterEnemy::Chams::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Enemy")][XORSTR("type")], (int*)&Settings::ESP::FilterEnemy::Chams::type);
    // For Local Player
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Localplayer")][XORSTR("enabled")], &Settings::ESP::FilterLocalPlayer::Chams::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Localplayer")][XORSTR("Fake")], (int*)&Settings::ESP::FilterLocalPlayer::Chams::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Localplayer")][XORSTR("Real")], (int*)&Settings::ESP::FilterLocalPlayer::RealChams::type);
    // For Alise
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Alise")][XORSTR("enabled")], &Settings::ESP::FilterAlise::Chams::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Alise")][XORSTR("type")], (int*)&Settings::ESP::FilterAlise::Chams::type);
    // END Visual Settings

    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Arms")][XORSTR("enabled")], &Settings::ESP::Chams::Arms::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Arms")][XORSTR("type")], (int*)&Settings::ESP::Chams::Arms::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Arms")][XORSTR("color")], &Settings::ESP::Chams::Arms::color);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Weapon")][XORSTR("enabled")], &Settings::ESP::Chams::Weapon::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Weapon")][XORSTR("type")], (int*)&Settings::ESP::Chams::Weapon::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("Weapon")][XORSTR("color")], &Settings::ESP::Chams::Weapon::color);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_ally_color")], &Settings::ESP::Chams::allyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_ally_visible_color")], &Settings::ESP::Chams::allyVisibleColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_enemy_color")], &Settings::ESP::Chams::enemyColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("players_enemy_visible_color")], &Settings::ESP::Chams::enemyVisibleColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("localplayer_color")], &Settings::ESP::Chams::localplayerColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("type")], (int*)&Settings::ESP::Chams::type);
    GetVal(settings[XORSTR("ESP")][XORSTR("Chams")][XORSTR("enabled")], &Settings::ESP::Chams::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("enabled")], &Settings::ESP::Sounds::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Sounds")][XORSTR("time")], &Settings::ESP::Sounds::time);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enabled")], &Settings::ESP::Hitmarker::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("enemies")], &Settings::ESP::Hitmarker::enemies);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("allies")], &Settings::ESP::Hitmarker::allies);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("color")], &Settings::ESP::Hitmarker::color);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("duration")], &Settings::ESP::Hitmarker::duration);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("size")], &Settings::ESP::Hitmarker::size);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("inner_gap")], &Settings::ESP::Hitmarker::innerGap);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Damage")][XORSTR("enabled")], &Settings::ESP::Hitmarker::Damage::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("enabled")], &Settings::ESP::Hitmarker::Sounds::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Hitmarker")][XORSTR("Sounds")][XORSTR("sound")], (int*)&Settings::ESP::Hitmarker::Sounds::sound);
    GetVal(settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("enabled")], &Settings::ESP::HeadDot::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("HeadDot")][XORSTR("size")], &Settings::ESP::HeadDot::size);
    GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("enabled")], &Settings::ESP::Spread::enabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("HitChance")], &Settings::ESP::Spread::spreadLimit);
    GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("color")], &Settings::ESP::Spread::color);
    GetVal(settings[XORSTR("ESP")][XORSTR("Spread")][XORSTR("spreadLimitColor")], &Settings::ESP::Spread::spreadLimitColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("indicators")], &Settings::ESP::indicators);

    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDist")], &Settings::ESP::DangerZone::drawDist);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drawDistEnabled")], &Settings::ESP::DangerZone::drawDistEnabled);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade")], &Settings::ESP::DangerZone::upgrade);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate")], &Settings::ESP::DangerZone::lootcrate);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("radarjammer")], &Settings::ESP::DangerZone::radarjammer);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel")], &Settings::ESP::DangerZone::barrel);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox")], &Settings::ESP::DangerZone::ammobox);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe")], &Settings::ESP::DangerZone::safe);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun")], &Settings::ESP::DangerZone::dronegun);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone")], &Settings::ESP::DangerZone::drone);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash")], &Settings::ESP::DangerZone::cash);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet")], &Settings::ESP::DangerZone::tablet);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot")], &Settings::ESP::DangerZone::healthshot);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee")], &Settings::ESP::DangerZone::melee);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("upgrade_color")], &Settings::ESP::DangerZone::upgradeColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("lootcrate_color")], &Settings::ESP::DangerZone::lootcrateColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("radarjammer_color")], &Settings::ESP::DangerZone::radarjammerColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("barrel_color")], &Settings::ESP::DangerZone::barrelColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("ammobox_color")], &Settings::ESP::DangerZone::ammoboxColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("safe_color")], &Settings::ESP::DangerZone::safeColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("dronegun_color")], &Settings::ESP::DangerZone::dronegunColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("drone_color")], &Settings::ESP::DangerZone::droneColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("cash_color")], &Settings::ESP::DangerZone::cashColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("tablet_color")], &Settings::ESP::DangerZone::tabletColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("healthshot_color")], &Settings::ESP::DangerZone::healthshotColor);
    GetVal(settings[XORSTR("ESP")][XORSTR("DangerZone")][XORSTR("melee_color")], &Settings::ESP::DangerZone::meleeColor);

    GetVal(settings[XORSTR("TracerEffects")][XORSTR("enabled")], &Settings::TracerEffects::enabled);
    GetVal(settings[XORSTR("TracerEffects")][XORSTR("serverSide")], &Settings::TracerEffects::serverSide);
    GetVal(settings[XORSTR("TracerEffects")][XORSTR("effect")], (int*)&Settings::TracerEffects::effect);
    GetVal(settings[XORSTR("TracerEffects")][XORSTR("frequency")], &Settings::TracerEffects::frequency);

    GetVal(settings[XORSTR("Dlights")][XORSTR("enabled")], &Settings::Dlights::enabled);
    GetVal(settings[XORSTR("Dlights")][XORSTR("radius")], &Settings::Dlights::radius);

    GetVal(settings[XORSTR("Spammer")][XORSTR("spammer_type")], (int*)&Settings::Spammer::type);
    GetVal(settings[XORSTR("Spammer")][XORSTR("say_team")], &Settings::Spammer::say_team);
    GetVal(settings[XORSTR("Spammer")][XORSTR("KillSpammer")][XORSTR("enabled")], &Settings::Spammer::KillSpammer::enabled);
    GetVal(settings[XORSTR("Spammer")][XORSTR("KillSpammer")][XORSTR("say_team")], &Settings::Spammer::KillSpammer::sayTeam);
    if (!settings[XORSTR("Spammer")][XORSTR("KillSpammer")][XORSTR("messages")].isNull())
    {
	Settings::Spammer::KillSpammer::messages.clear();
	for (const Json::Value& message : settings[XORSTR("Spammer")][XORSTR("KillSpammer")][XORSTR("messages")])
	    Settings::Spammer::KillSpammer::messages.push_back(message.asString());
    }
    if (!settings[XORSTR("Spammer")][XORSTR("NormalSpammer")][XORSTR("messages")].isNull())
    {
	Settings::Spammer::NormalSpammer::messages.clear();
	for (const Json::Value& message : settings[XORSTR("Spammer")][XORSTR("NormalSpammer")][XORSTR("messages")])
	    Settings::Spammer::NormalSpammer::messages.push_back(message.asString());
    }
    GetVal(settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_name")], &Settings::Spammer::PositionSpammer::showName);
    GetVal(settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_weapon")], &Settings::Spammer::PositionSpammer::showWeapon);
    GetVal(settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_rank")], &Settings::Spammer::PositionSpammer::showRank);
    GetVal(settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_wins")], &Settings::Spammer::PositionSpammer::showWins);
    GetVal(settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_health")], &Settings::Spammer::PositionSpammer::showHealth);
    GetVal(settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_money")], &Settings::Spammer::PositionSpammer::showMoney);
    GetVal(settings[XORSTR("Spammer")][XORSTR("PositionSpammer")][XORSTR("show_lastplace")], &Settings::Spammer::PositionSpammer::showLastplace);

    GetVal(settings[XORSTR("BHop")][XORSTR("enabled")], &Settings::BHop::enabled);
    GetVal(settings[XORSTR("BHop")][XORSTR("Chance")][XORSTR("enabled")], &Settings::BHop::Chance::enabled);
    GetVal(settings[XORSTR("BHop")][XORSTR("Chance")][XORSTR("value")], &Settings::BHop::Chance::value);
    GetVal(settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("enabledMax")], &Settings::BHop::Hops::enabledMax);
    GetVal(settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("Max")], &Settings::BHop::Hops::Max);
    GetVal(settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("enabledMin")], &Settings::BHop::Hops::enabledMin);
    GetVal(settings[XORSTR("BHop")][XORSTR("Hops")][XORSTR("Min")], &Settings::BHop::Hops::Min);

    GetVal(settings[XORSTR("NoDuckCooldown")][XORSTR("enabled")], &Settings::NoDuckCooldown::enabled);

    GetVal(settings[XORSTR("AutoStrafe")][XORSTR("enabled")], &Settings::AutoStrafe::enabled);
    GetVal(settings[XORSTR("AutoStrafe")][XORSTR("type")], (int*)&Settings::AutoStrafe::type);
    GetVal(settings[XORSTR("AutoStrafe")][XORSTR("silent")], &Settings::AutoStrafe::silent);

    GetVal(settings[XORSTR("Noflash")][XORSTR("enabled")], &Settings::Noflash::enabled);
    GetVal(settings[XORSTR("Noflash")][XORSTR("value")], &Settings::Noflash::value);

    GetVal(settings[XORSTR("Radar")][XORSTR("enabled")], &Settings::Radar::enabled);
    GetVal(settings[XORSTR("Radar")][XORSTR("zoom")], &Settings::Radar::zoom);
    GetVal(settings[XORSTR("Radar")][XORSTR("enemies")], &Settings::Radar::enemies);
    GetVal(settings[XORSTR("Radar")][XORSTR("allies")], &Settings::Radar::allies);
    GetVal(settings[XORSTR("Radar")][XORSTR("legit")], &Settings::Radar::legit);
    GetVal(settings[XORSTR("Radar")][XORSTR("visibility_check")], &Settings::Radar::visibilityCheck);
    GetVal(settings[XORSTR("Radar")][XORSTR("smoke_check")], &Settings::Radar::smokeCheck);
    GetVal(settings[XORSTR("Radar")][XORSTR("InGame")][XORSTR("enabled")], &Settings::Radar::InGame::enabled);
    GetVal(settings[XORSTR("Radar")][XORSTR("pos")][XORSTR("x")], &Settings::Radar::pos.x);
    GetVal(settings[XORSTR("Radar")][XORSTR("pos")][XORSTR("y")], &Settings::Radar::pos.y);
    GetVal(settings[XORSTR("Radar")][XORSTR("enemy_color")], &Settings::Radar::enemyColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("enemy_visible_color")], &Settings::Radar::enemyVisibleColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("ally_color")], &Settings::Radar::allyColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("ally_visible_color")], &Settings::Radar::allyVisibleColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("t_color")], &Settings::Radar::tColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("t_visible_color")], &Settings::Radar::tVisibleColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("ct_color")], &Settings::Radar::ctColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("ct_visible_color")], &Settings::Radar::ctVisibleColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("bomb_color")], &Settings::Radar::bombColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("bomb_defusing_color")], &Settings::Radar::bombDefusingColor);
    GetVal(settings[XORSTR("Radar")][XORSTR("icons_scale")], &Settings::Radar::iconsScale);

    GetVal(settings[XORSTR("Recoilcrosshair")][XORSTR("enabled")], &Settings::Recoilcrosshair::enabled);
    GetVal(settings[XORSTR("Recoilcrosshair")][XORSTR("showOnlyWhenShooting")], &Settings::Recoilcrosshair::showOnlyWhenShooting);

    GetVal(settings[XORSTR("FOVChanger")][XORSTR("enabled")], &Settings::FOVChanger::enabled);
    GetVal(settings[XORSTR("FOVChanger")][XORSTR("value")], &Settings::FOVChanger::value);
    GetVal(settings[XORSTR("FOVChanger")][XORSTR("viewmodel_enabled")], &Settings::FOVChanger::viewmodelEnabled);
    GetVal(settings[XORSTR("FOVChanger")][XORSTR("viewmodel_value")], &Settings::FOVChanger::viewmodelValue);
    GetVal(settings[XORSTR("FOVChanger")][XORSTR("ignore_scope")], &Settings::FOVChanger::ignoreScope);

    Settings::Skinchanger::Skins::enabled = false;
    Settings::Skinchanger::skinsCT.clear();
    Settings::Skinchanger::skinsT.clear();

    for (Json::ValueIterator itr = settings[XORSTR("SkinChanger")][XORSTR("skinsCT")].begin(); itr != settings[XORSTR("SkinChanger")][XORSTR("skinsCT")].end(); itr++)
    {
	std::string skinDataKey = itr.key().asString();
	auto skinSetting = settings[XORSTR("SkinChanger")][XORSTR("skinsCT")][skinDataKey];

	// XXX Using exception handling to deal with this is stupid, but I don't care to find a better solution
	// XXX We can't use GetOrdinal() since the key type is a string...
	unsigned int weaponID;

	try
	{
	    weaponID = std::stoi(skinDataKey);
	}
	catch (std::invalid_argument&)
	{
	    weaponID = (int)Util::Items::GetItemIndex(skinDataKey);
	}

	ItemDefinitionIndex defIndex = ItemDefinitionIndex::INVALID;
	GetOrdinal<ItemDefinitionIndex, Util::Items::GetItemIndex>(skinSetting[XORSTR("ItemDefinitionIndex")], &defIndex);

	if (Settings::Skinchanger::skinsCT.find((ItemDefinitionIndex)weaponID) == Settings::Skinchanger::skinsCT.end())
	    Settings::Skinchanger::skinsCT[(ItemDefinitionIndex)weaponID] = AttribItem_t();

	AttribItem_t skin = {
	    defIndex,
	    skinSetting[XORSTR("PaintKit")].asInt(),
	    skinSetting[XORSTR("Wear")].asFloat(),
	    skinSetting[XORSTR("Seed")].asInt(),
	    skinSetting[XORSTR("StatTrak")].asInt(),
	    -1,
	    skinSetting[XORSTR("CustomName")].asString(),
	};

	Settings::Skinchanger::skinsCT.at((ItemDefinitionIndex)weaponID) = skin;
    }

    for (Json::ValueIterator itr = settings[XORSTR("SkinChanger")][XORSTR("skinsT")].begin(); itr != settings[XORSTR("SkinChanger")][XORSTR("skinsT")].end(); itr++)
    {
	std::string skinDataKey = itr.key().asString();
	auto skinSetting = settings[XORSTR("SkinChanger")][XORSTR("skinsT")][skinDataKey];

	// XXX Using exception handling to deal with this is stupid, but I don't care to find a better solution
	// XXX We can't use GetOrdinal() since the key type is a string...
	unsigned int weaponID;

	try
	{
	    weaponID = std::stoi(skinDataKey);
	}
	catch (std::invalid_argument&)
	{
	    weaponID = (int)Util::Items::GetItemIndex(skinDataKey);
	}

	ItemDefinitionIndex defIndex = ItemDefinitionIndex::INVALID;
	GetOrdinal<ItemDefinitionIndex, Util::Items::GetItemIndex>(skinSetting[XORSTR("ItemDefinitionIndex")], &defIndex);

	if (Settings::Skinchanger::skinsT.find((ItemDefinitionIndex)weaponID) == Settings::Skinchanger::skinsT.end())
	    Settings::Skinchanger::skinsT[(ItemDefinitionIndex)weaponID] = AttribItem_t();

	AttribItem_t skin = {
	    defIndex,
	    skinSetting[XORSTR("PaintKit")].asInt(),
	    skinSetting[XORSTR("Wear")].asFloat(),
	    skinSetting[XORSTR("Seed")].asInt(),
	    skinSetting[XORSTR("StatTrak")].asInt(),
	    -1,
	    skinSetting[XORSTR("CustomName")].asString(),
	};

	Settings::Skinchanger::skinsT.at((ItemDefinitionIndex)weaponID) = skin;
    }

    SkinChanger::forceFullUpdate = true;

    GetVal(settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("enabled")], &Settings::Skinchanger::Skins::enabled);
    GetVal(settings[XORSTR("SkinChanger")][XORSTR("Models")][XORSTR("enabled")], &Settings::Skinchanger::Models::enabled);
    GetVal(settings[XORSTR("SkinChanger")][XORSTR("Skins")][XORSTR("perTeam")], &Settings::Skinchanger::Skins::perTeam);

    GetVal(settings[XORSTR("ShowRanks")][XORSTR("enabled")], &Settings::ShowRanks::enabled);

    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posX")], &Settings::UI::Windows::Colors::posX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("posY")], &Settings::UI::Windows::Colors::posY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeX")], &Settings::UI::Windows::Colors::sizeX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("sizeY")], &Settings::UI::Windows::Colors::sizeY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Colors")][XORSTR("open")], &Settings::UI::Windows::Colors::open);

    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posX")], &Settings::UI::Windows::Config::posX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("posY")], &Settings::UI::Windows::Config::posY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeX")], &Settings::UI::Windows::Config::sizeX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("sizeY")], &Settings::UI::Windows::Config::sizeY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Config")][XORSTR("open")], &Settings::UI::Windows::Config::open);

    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posX")], &Settings::UI::Windows::Main::posX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("posY")], &Settings::UI::Windows::Main::posY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeX")], &Settings::UI::Windows::Main::sizeX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("sizeY")], &Settings::UI::Windows::Main::sizeY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Main")][XORSTR("open")], &Settings::UI::Windows::Main::open);

    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("posX")], &Settings::UI::Windows::Playerlist::posX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("posY")], &Settings::UI::Windows::Playerlist::posY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("sizeX")], &Settings::UI::Windows::Playerlist::sizeX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("sizeY")], &Settings::UI::Windows::Playerlist::sizeY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Playerlist")][XORSTR("open")], &Settings::UI::Windows::Playerlist::open);

    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posX")], &Settings::UI::Windows::Skinmodel::posX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("posY")], &Settings::UI::Windows::Skinmodel::posY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeX")], &Settings::UI::Windows::Skinmodel::sizeX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("sizeY")], &Settings::UI::Windows::Skinmodel::sizeY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Skinmodel")][XORSTR("open")], &Settings::UI::Windows::Skinmodel::open);

    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posX")], &Settings::UI::Windows::Spectators::posX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("posY")], &Settings::UI::Windows::Spectators::posY);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeX")], &Settings::UI::Windows::Spectators::sizeX);
    GetVal(settings[XORSTR("UI")][XORSTR("Windows")][XORSTR("Spectators")][XORSTR("sizeY")], &Settings::UI::Windows::Spectators::sizeY);

    Settings::UI::Windows::Main::reload = true;
    Settings::UI::Windows::Colors::reload = true;
    Settings::UI::Windows::Config::reload = true;
    Settings::UI::Windows::Playerlist::reload = true;
    Settings::UI::Windows::Skinmodel::reload = true;
    Settings::UI::Windows::Spectators::reload = true;

    GetVal(settings[XORSTR("ShowSpectators")][XORSTR("enabled")], &Settings::ShowSpectators::enabled);

    GetVal(settings[XORSTR("ClanTagChanger")][XORSTR("value")], (char*)&Settings::ClanTagChanger::value);
    GetVal(settings[XORSTR("ClanTagChanger")][XORSTR("enabled")], &Settings::ClanTagChanger::enabled);
    GetVal(settings[XORSTR("ClanTagChanger")][XORSTR("animation")], &Settings::ClanTagChanger::animation);
    GetVal(settings[XORSTR("ClanTagChanger")][XORSTR("animation_speed")], &Settings::ClanTagChanger::animationSpeed);
    GetVal(settings[XORSTR("ClanTagChanger")][XORSTR("type")], (int*)&Settings::ClanTagChanger::type);
    ::ClanTagChanger::UpdateClanTagCallback();

    GetVal(settings[XORSTR("View")][XORSTR("NoViewPunch")][XORSTR("enabled")], &Settings::View::NoViewPunch::enabled);
    GetVal(settings[XORSTR("View")][XORSTR("NoAimPunch")][XORSTR("enabled")], &Settings::View::NoAimPunch::enabled);
    GetVal(settings[XORSTR("FakeWalk")][XORSTR("enabled")], &Settings::FakeWalk::enabled);

    GetVal(settings[XORSTR("FakeLag")][XORSTR("enabled")], &Settings::FakeLag::enabled);
    GetVal(settings[XORSTR("FakeLag")][XORSTR("value")], &Settings::FakeLag::value);
    GetVal(settings[XORSTR("FakeLag")][XORSTR("adaptive")], &Settings::FakeLag::adaptive);
    GetVal(settings[XORSTR("FakeLag")][XORSTR("randomLag")], &Settings::AntiAim::randomLag::enabled);

    GetVal(settings[XORSTR("SlowWalk")][XORSTR("enabled")], &Settings::AntiAim::SlowWalk::enabled);
    GetButtonCode(settings[XORSTR("SlowWalk")][XORSTR("key")], &Settings::AntiAim::SlowWalk::key);

    GetVal(settings[XORSTR("AutoAccept")][XORSTR("enabled")], &Settings::AutoAccept::enabled);

    GetVal(settings[XORSTR("NoSky")][XORSTR("enabled")], &Settings::NoSky::enabled);
    GetVal(settings[XORSTR("NoSky")][XORSTR("color")], &Settings::NoSky::color);

    GetVal(settings[XORSTR("SkyBox")][XORSTR("enabled")], &Settings::SkyBox::enabled);
    GetVal(settings[XORSTR("SkyBox")][XORSTR("skyBoxNumber")], &Settings::SkyBox::skyBoxNumber);

    GetVal(settings[XORSTR("ASUSWalls")][XORSTR("enabled")], &Settings::ASUSWalls::enabled);
    GetVal(settings[XORSTR("ASUSWalls")][XORSTR("color")], &Settings::ASUSWalls::color);

    GetVal(settings[XORSTR("NoScopeBorder")][XORSTR("enabled")], &Settings::NoScopeBorder::enabled);

    GetVal(settings[XORSTR("SniperCrosshair")][XORSTR("enabled")], &Settings::SniperCrosshair::enabled);

    GetVal(settings[XORSTR("Autoblock")][XORSTR("enabled")], &Settings::Autoblock::enabled);
    GetButtonCode(settings[XORSTR("Autoblock")][XORSTR("key")], &Settings::Autoblock::key);

    GetVal(settings[XORSTR("AutoDefuse")][XORSTR("enabled")], &Settings::AutoDefuse::enabled);
    GetVal(settings[XORSTR("AutoDefuse")][XORSTR("silent")], &Settings::AutoDefuse::silent);

    GetVal(settings[XORSTR("NoSmoke")][XORSTR("enabled")], &Settings::NoSmoke::enabled);
    GetVal(settings[XORSTR("NoSmoke")][XORSTR("type")], (int*)&Settings::NoSmoke::type);

    GetVal(settings[XORSTR("ScreenshotCleaner")][XORSTR("enabled")], &Settings::ScreenshotCleaner::enabled);

    GetVal(settings[XORSTR("EdgeJump")][XORSTR("enabled")], &Settings::EdgeJump::enabled);
    GetButtonCode(settings[XORSTR("EdgeJump")][XORSTR("key")], &Settings::EdgeJump::key);

    GetVal(settings[XORSTR("NameStealer")][XORSTR("enabled")], &Settings::NameStealer::enabled);
    GetVal(settings[XORSTR("NameStealer")][XORSTR("team")], &Settings::NameStealer::team);

    GetVal(settings[XORSTR("Eventlog")][XORSTR("showEnemies")], &Settings::Eventlog::showEnemies);
    GetVal(settings[XORSTR("Eventlog")][XORSTR("showTeammates")], &Settings::Eventlog::showTeammates);
    GetVal(settings[XORSTR("Eventlog")][XORSTR("showLocalplayer")], &Settings::Eventlog::showLocalplayer);
    GetVal(settings[XORSTR("Eventlog")][XORSTR("duration")], &Settings::Eventlog::duration);
    GetVal(settings[XORSTR("Eventlog")][XORSTR("lines")], &Settings::Eventlog::lines);
    GetVal(settings[XORSTR("Eventlog")][XORSTR("color")], &Settings::Eventlog::color);

    GetVal(settings[XORSTR("ThirdPerson")][XORSTR("enabled")], &Settings::ThirdPerson::enabled);
    GetVal(settings[XORSTR("ThirdPerson")][XORSTR("toggled")], &Settings::ThirdPerson::toggled);
    GetVal(settings[XORSTR("ThirdPerson")][XORSTR("distance")], &Settings::ThirdPerson::distance);
    GetButtonCode(settings[XORSTR("ThirdPerson")][XORSTR("togglekey")], &Settings::ThirdPerson::toggleThirdPerson);

    GetVal(settings[XORSTR("JumpThrow")][XORSTR("enabled")], &Settings::JumpThrow::enabled);
    GetButtonCode(settings[XORSTR("JumpThrow")][XORSTR("key")], &Settings::JumpThrow::key);

    GetVal(settings[XORSTR("DisablePostProcessing")][XORSTR("enabled")], &Settings::DisablePostProcessing::enabled);

    GetVal(settings[XORSTR("NoFall")][XORSTR("enabled")], &Settings::NoFall::enabled);
    GetVal(settings[XORSTR("RagdollGravity")][XORSTR("enable")], &Settings::RagdollGravity::enabled);

    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("enabled")], &Settings::GrenadeHelper::enabled);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("aimAssist")], &Settings::GrenadeHelper::aimAssist);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("OnlyMatching")], &Settings::GrenadeHelper::onlyMatchingInfos);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("aimStep")], &Settings::GrenadeHelper::aimStep);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("aimDistance")], &Settings::GrenadeHelper::aimDistance);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("aimFov")], &Settings::GrenadeHelper::aimFov);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("aimDot")], &Settings::GrenadeHelper::aimDot);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("aimLine")], &Settings::GrenadeHelper::aimLine);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("infoHE")], &Settings::GrenadeHelper::infoHE);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("infoSmoke")], &Settings::GrenadeHelper::infoSmoke);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("infoFlash")], &Settings::GrenadeHelper::infoFlash);
    GetVal(settings[XORSTR("GrenadeHelper")][XORSTR("infoMolotov")], &Settings::GrenadeHelper::infoMolotov);

    GetVal(settings[XORSTR("GrenadePrediction")][XORSTR("enabled")], &Settings::GrenadePrediction::enabled);
    GetVal(settings[XORSTR("GrenadePrediction")][XORSTR("color")], &Settings::GrenadePrediction::color);

    GetVal(settings[XORSTR("AutoKnife")][XORSTR("enabled")], &Settings::AutoKnife::enabled);
    GetVal(settings[XORSTR("AutoKnife")][XORSTR("Filters")][XORSTR("enemies")], &Settings::AutoKnife::Filters::enemies);
    GetVal(settings[XORSTR("AutoKnife")][XORSTR("Filters")][XORSTR("allies")], &Settings::AutoKnife::Filters::allies);
    GetVal(settings[XORSTR("AutoKnife")][XORSTR("onKey")], &Settings::AutoKnife::onKey);
 	GetVal(settings[XORSTR("QuickSwitch")][XORSTR("enabled")], &Settings::QuickSwitch::enabled);
}

void Settings::SaveGrenadeInfo(std::string path)
{
    Json::Value grenadeInfos;
    for (auto grenadeInfo = GrenadeHelper::grenadeInfos.begin(); grenadeInfo != GrenadeHelper::grenadeInfos.end(); grenadeInfo++)
    {
	Json::Value act;
	act[XORSTR("name")] = grenadeInfo->name.c_str();
	act[XORSTR("gType")] = grenadeInfo->gType;
	act[XORSTR("tType")] = grenadeInfo->tType;
	act[XORSTR("pos")][XORSTR("x")] = grenadeInfo->pos.x;
	act[XORSTR("pos")][XORSTR("y")] = grenadeInfo->pos.y;
	act[XORSTR("pos")][XORSTR("z")] = grenadeInfo->pos.z;

	act[XORSTR("angle")][XORSTR("x")] = grenadeInfo->angle.x;
	act[XORSTR("angle")][XORSTR("y")] = grenadeInfo->angle.y;

	grenadeInfos.append(act);
    }

    Json::Value data;
    Json::StyledWriter styledWriter;

    data[XORSTR("smokeinfos")] = grenadeInfos;

    std::ofstream(path) << styledWriter.write(data);
}

void Settings::LoadGrenadeInfo(std::string path)
{
    if (!std::ifstream(path).good() || !DoesFileExist(path.c_str()))
	return;
    Json::Value data;
    std::ifstream configDoc(path, std::ifstream::binary);
    try
    {
	configDoc >> data;
    }
    catch (...)
    {
	cvar->ConsoleDPrintf(XORSTR("Error parsing the config file.\n"));
	return;
    }

    Json::Value array = data[XORSTR("smokeinfos")];
    Settings::GrenadeHelper::grenadeInfos = {};

    for (Json::Value::iterator it = array.begin(); it != array.end(); ++it)
    {
	Json::Value& act = *it;

	const char* name = act[XORSTR("name")].asCString();

	GrenadeType gType = (GrenadeType)act[XORSTR("gType")].asInt();
	ThrowType tType = (ThrowType)act[XORSTR("tType")].asInt();
	Json::Value pos = act[XORSTR("pos")];
	Vector posVec(pos[XORSTR("x")].asFloat(), pos[XORSTR("y")].asFloat(), pos[XORSTR("z")].asFloat());
	Json::Value angle = act[XORSTR("angle")];
	QAngle vAngle(angle[XORSTR("x")].asFloat(), angle[XORSTR("y")].asFloat(), 0.f);
	Settings::GrenadeHelper::grenadeInfos.emplace_back(gType, posVec, vAngle, tType, name);
    }
}

void remove_directory(const char* path)
{
    DIR* dir = opendir(path);
    dirent* pdir;

    const size_t path_len = strlen(path);

    while ((pdir = readdir(dir)))
    {
	if (strcmp(pdir->d_name, ".") == 0 || strcmp(pdir->d_name, "..") == 0)
	    continue;

	std::string _path;
	_path.reserve(path_len + Util::StrLen("/") + strlen(pdir->d_name));
	_path.append(path);
	_path += '/';
	_path.append(pdir->d_name);

	if (pdir->d_type == DT_DIR)
	{
	    remove_directory(_path.c_str());
	}
	else if (pdir->d_type == DT_REG)
	{
	    unlink(_path.c_str());
	}
    }

    rmdir(path);
}

void Settings::DeleteConfig(std::string path)
{
    remove_directory(path.c_str());
}
