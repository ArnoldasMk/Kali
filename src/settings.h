#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "ImGUI/imgui.h"
#include "Utils/util_sdk.h"
#include "Utils/util_items.h"
#include "Utils/util.h"

#include "SDK/IClientEntity.h"
#include "SDK/IInputSystem.h"
#include "SDK/definitions.h"
#include "SDK/Materialsystem_config.h"

enum class DamagePrediction : int {
	justDamage = 0,
	damage,
};

enum class EnemySelectionType : int {
	BestDamage = 0,
	CLosestToCrosshair,
};

enum class DesireBones : int {
	BONE_HEAD = 0,
	UPPER_CHEST,
	MIDDLE_CHEST,
	LOWER_CHEST,
	BONE_HIP,
	LOWER_BODY,
};
enum class DrawingBackend : int {
    SURFACE = 0,
    IMGUI,
    NUMBER_OF_TYPES,
};

enum class SmoothType : int
{
	SLOW_END,
	CONSTANT,
	FAST_END,
};

enum class TracerType : int
{
	BOTTOM,
	CURSOR,
	ARROWS,
};

enum class ClanTagType : int
{
	STATIC,
	MARQUEE,
	WORDS,
	LETTERS,
};

enum class impactType : int
{
        ITSME,
        PRITAM,
};

enum class MusicType : int
{
        CSGO,
        CSGO2,
        CRIMSON_ASSAULT,
        SHARPENED,
	INSURGENCY,
	ADB,
	HIGH_MOON,
	DEATHS_HEAD_DEMOLITION,
	DESERT_FIRE,
	LNOE,
	METAL,
	ALL_I_WANT_FOR_CHRISTMAS,
	ISORHYTHM,
	FOR_NO_MANKIND,
	HOTLINE_MIAMI,
	TOTAL_DOMINATION,
	THE_TALOS_PRINCIPLE,
	BATTLEPACK,
	MOLOTOV,
	UBER_BLASTO_PHONE,
	HAZARDOUS_ENVIRONMENTS,
	II_HEADSHOT,
	THE_8_BIT_KIT,
	I_AM,
	DIAMONDS,
	INVASION,
	LIONS_MOUTH,
	SPONGE_FINGERZ,
	DISGUSTING,
	JAVA_HAVANA_FUNKALOO,
	MOMENTS_CSGO,
	AGGRESSIVE,
	THE_GOOD_YOUTH,
	FREE,
	LIFES_NOT_OUT_TO_GET_YOU,
	BACKBONE,
	GLA,
	III_ARENA,
	EZ4ENCE,
};

enum class AutostrafeType : int
{
	AS_FORWARDS,
	AS_BACKWARDS,
	AS_LEFTSIDEWAYS,
	AS_RIGHTSIDEWAYS,
	AS_RAGE,
	AS_DIRECTIONAL,
};

enum class ChamsType : int
{
	WHITEADDTIVE,
	ADDITIVETWO,
	WIREFRAME,
	FLAT,
	PEARL,
	GLOW,
	GLOWF,
	PULSE,
	NONE,
};

enum class Filter : int
{
	Enemies,
	LocalPlayer,
	Alise,
	Fish,
	Chicken,
};

enum class BoxType : int
{
	FLAT_2D,
	FRAME_2D,
	BOX_3D,
	HITBOXES,
};

enum class SpriteType : int
{
	SPRITE_TUX,
};
enum class AntiAimType_X : int
{
    STATIC_UP,
    STATIC_DOWN,
    FRONT,
    FRONT_FAKE,
};
enum class Sound : int {

	NONE,
	SPONGEBOB,
	HALFLIFE,
	HALFLIFE2,
	HALFLIFE3,
	HALFLIFE4,
	BBGUNBELL,
	DOPAMINE,
	WUB,
	PEDOYES,
	MEME,
	ERROR,
	ORCHESTRAL,
	GAMESENSE,
};

enum class BarType : int
{
	VERTICAL,
	VERTICAL_RIGHT,
	HORIZONTAL,
	HORIZONTAL_UP,
	BATTERY,
};

enum class BarColorType : int
{
	STATIC_COLOR,
	HEALTH_BASED,
};

enum class TeamColorType : int
{
	ABSOLUTE,
	RELATIVE,
};

enum class SmokeType : int
{
	WIREFRAME,
	NONE,
};

enum class AimTargetType : int
{
	FOV,
	DISTANCE,
	REAL_DISTANCE,
	HP,
};

enum class DtSpeed : int
{
        INSTANT,
        FAST,
        ACCURATE,
};

enum class SpammerType : int
{
	SPAMMER_NONE,
	SPAMMER_NORMAL,
	SPAMMER_POSITIONS,
};

enum class resolverType : int
{
        NONE,
        Experimental,
        ApuWare,
};

enum class AntiAimRealType_Y : int
{
	NONE,
	Static,
	Jitter,
	Randome,
	JitterSwitch,
	JitterRandom,
};

enum class AntiAimFakeType_y : int
{
	NONE,
	Static,
	Jitter,
	Randome,
};

enum class AntiAimType : int
{
	LegitAntiAim,
	RageAntiAim,
	Lagacy,
};

enum class LbyMode : int
{
	Normal,
	Opposite,
};

enum class RageAntiAimType : int
{
	DefaultRage,
	FreeStand,
};

enum class LegitAAType : int
{
    OverWatchProof,
	FakeLegitAA,
	Experimental,
};

enum class SkinAndModel : int
{
	Skins,
	Model,
};

struct LegitWeapon_t
{
	bool silent,
		 autoShoot,
		 aimkeyOnly,
		 smoothEnabled,
		 smoothSaltEnabled,
		 errorMarginEnabled,
		 autoAimEnabled,
		 aimStepEnabled,
		 rcsEnabled,
		 rcsAlwaysOn,
		 hitchanceEnaled,
		 autoPistolEnabled,
		 autoScopeEnabled,
		 ignoreJumpEnabled,
		 ignoreEnemyJumpEnabled,
		 autoSlow,
		 predEnabled,
		 TriggerBot,
		 mindamage,
		 autoWall;
	Bone bone = BONE_HEAD;
	SmoothType smoothType = SmoothType::SLOW_END;
	ButtonCode_t aimkey = ButtonCode_t ::MOUSE_MIDDLE;
	float smoothAmount = 1.0f,
		  smoothSaltMultiplier = 0.0f,
		  errorMarginValue = 0.0f,
		  LegitautoAimFov = 15.0f,
		  aimStepMin = 25.0f,
		  aimStepMax = 35.0f,
		  rcsAmountX = 2.0f,
		  rcsAmountY = 2.0f,
		  minDamagevalue = 10.0f,
		  hitchance = 20;
	bool desiredBones[31];

	bool operator == (const LegitWeapon_t& another) const
	{
		for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		{
			if( this->desiredBones[bone] != another.desiredBones[bone] )
				return false;
		}

		return this->silent == another.silent &&
			this->autoShoot == another.autoShoot &&
			this->bone == another.bone &&
			this->aimkey == another.aimkey &&
			this->aimkeyOnly == another.aimkeyOnly &&
			this->smoothEnabled == another.smoothEnabled &&
			this->smoothAmount == another.smoothAmount &&
			this->smoothType == another.smoothType &&
			this->smoothSaltEnabled == another.smoothSaltEnabled &&
			this->smoothSaltMultiplier == another.smoothSaltMultiplier &&
			this->errorMarginEnabled == another.errorMarginEnabled &&
			this->errorMarginValue == another.errorMarginValue &&
			this->autoAimEnabled == another.autoAimEnabled &&
			this->LegitautoAimFov == another.LegitautoAimFov &&
			this->aimStepEnabled == another.aimStepEnabled &&
			this->aimStepMin == another.aimStepMin &&
			this->aimStepMax == another.aimStepMax &&
			this->rcsEnabled == another.rcsEnabled &&
			this->rcsAlwaysOn == another.rcsAlwaysOn &&
			this->rcsAmountX == another.rcsAmountX &&
			this->rcsAmountY == another.rcsAmountY &&
			this->autoPistolEnabled == another.autoPistolEnabled &&
			this->autoScopeEnabled == another.autoScopeEnabled &&
			this->ignoreEnemyJumpEnabled == another.ignoreEnemyJumpEnabled &&
			this->hitchanceEnaled == another.hitchanceEnaled &&
			this->hitchance == another.hitchance &&
			this->autoSlow == another.autoSlow &&
			this->predEnabled == another.predEnabled &&
			this->TriggerBot == another.TriggerBot &&
			this->mindamage == another.mindamage &&
			this->autoWall == another.autoWall;
	}
} const defaultSettings{};

struct RageWeapon_t
{
	bool silent,
		 friendly,
		 closestBone,
		 HitChanceEnabled,
		 autoPistolEnabled,
		 autoShootEnabled,
		 autoScopeEnabled,
		 autoSlow,
		 scopeControlEnabled,
		 DoubleFire;
	float MinDamage = 50.f,
		  HitChance = 20.f;
	DamagePrediction DmagePredictionType = DamagePrediction::damage;
	EnemySelectionType enemySelectionType = EnemySelectionType::CLosestToCrosshair;
	bool desireBones[6];
	bool desiredMultiBones[6];
	

	bool operator == (const RageWeapon_t& Ragebotanother) const
	{
		for (int bone = 0; bone < 6; bone++) // static bones
		{
			if( this->desireBones[bone] != Ragebotanother.desireBones[bone] )
				return false;
			if( this->desiredMultiBones[bone] != Ragebotanother.desiredMultiBones[bone] )
				return false;
		}
			

		return this->silent == Ragebotanother.silent &&
			this->friendly == Ragebotanother.friendly &&
			this->closestBone == Ragebotanother.closestBone &&
			this->autoPistolEnabled == Ragebotanother.autoPistolEnabled &&
			this->autoShootEnabled == Ragebotanother.autoShootEnabled &&
			this->autoScopeEnabled == Ragebotanother.autoScopeEnabled &&
			this->HitChanceEnabled == Ragebotanother.HitChanceEnabled &&
			this->MinDamage == Ragebotanother.MinDamage &&
			this->autoSlow == Ragebotanother.autoSlow &&
			this->scopeControlEnabled == Ragebotanother.scopeControlEnabled && 
			this->HitChance == Ragebotanother.HitChance && 
			this->DmagePredictionType == Ragebotanother.DmagePredictionType && 
			this->enemySelectionType == Ragebotanother.enemySelectionType && 
			this->DoubleFire == Ragebotanother.DoubleFire;
	}

} const ragedefault{};



class ColorVar
{
public:
	ImColor color;
	ImVec4 ColorVec4;
	bool rainbow;
	float rainbowSpeed;

	ColorVar() {}

	ColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
		this->ColorVec4 = getVec4();
	}

	ImColor Color()
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : this->color;
		result.Value.w = this->color.Value.w;
		return result;
	}

	ImVec4 getVec4()
	{
		return color.operator ImVec4();
	}
};

class HealthColorVar : public ColorVar
{
public:
	bool hp;

	HealthColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
		this->hp = false;
	}

	ImColor Color(C_BasePlayer* player)
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : (this->hp ? Color::ToImColor(Util::GetHealthColor(player)) : this->color);
		result.Value.w = this->color.Value.w;
		return result;
	}
};

namespace Settings
{
	namespace UI
	{
		inline ColorVar mainColor = ImColor(42, 45, 42, 255 );
		inline ColorVar bodyColor = ImColor( 0, 0, 0, 245 );
		inline ColorVar fontColor = ImColor( 238, 224, 224, 154 );
		inline ColorVar accentColor = ImColor( 0, 171, 178, 255 );
		inline bool imGuiAliasedLines = false;
		inline bool imGuiAliasedFill = true;

        /* Window Position/Size Defaults */
        namespace Windows
		{
			namespace Colors
			{
				inline int posX = 540;
				inline int posY = 325;
				inline int sizeX = 540;
				inline int sizeY = 360;
				inline bool open = false;
				inline bool reload = false ; // True on config load, used to change Window Properties.
			}
			namespace Config
			{
				inline int posX = 185;
				inline int posY = 250;
				inline int sizeX = 185;
				inline int sizeY = 250;
				inline bool open = false;
				inline bool reload = false;
			}
			namespace Main
			{
				inline int posX = 30;
				inline int posY = 20;
				inline int sizeX = 960;
				inline int sizeY = 645;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Playerlist
			{
				inline int posX = 700;
				inline int posY = 500;
				inline int sizeX = 700;
				inline int sizeY = 500;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Skinmodel
			{
				inline int posX = 1050;
				inline int posY = 645;
				inline int sizeX = 1050;
				inline int sizeY = 645;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Kills
                        {
                                inline int posX = 50;
                                inline int posY = 100;
                                inline int sizeX = 50;
                                inline int sizeY = 100;
                                inline bool reload = false; // True on config load, used to change Window Position.
                        }

			namespace Spectators
			{
				inline int posX = 50;
				inline int posY = 100;
				inline int sizeX = 50;
				inline int sizeY = 100;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
		}
		namespace Fonts
		{
			namespace ESP
			{
				inline char* family = (char*)"Verdana";
				inline int size = 12;
				inline int flags = (int)FontFlags::FONTFLAG_OUTLINE;
			}
		}
	}
    /* Default LegitBot Settings */
	namespace Legitbot
	{
		inline bool enabled = false;
        inline bool silent = false;
        inline bool friendly = false;
		inline float minDamage = 10.f;
        inline Bone bone = BONE_HEAD;
        inline ButtonCode_t aimkey = ButtonCode_t::MOUSE_MIDDLE;
        inline bool aimkeyOnly = false;

		namespace Smooth
		{
			inline bool enabled = false;
            inline float value = 0.5f;
            inline SmoothType type = SmoothType::SLOW_END;

			namespace Salting
			{
				inline bool enabled = false;
                inline float multiplier = 0.0f;
			}
		}

		namespace ErrorMargin
		{
			inline bool enabled = false;
			inline float value = 0.0f;
		}

		namespace AutoAim
		{
			inline bool enabled = false;
            inline float fov = 15.f;
            inline bool realDistance = false;
            inline bool closestBone = false;
            inline bool desiredBones[] = {true, true, true, true, true, true, true, // center mass
                                          false, false, false, false, false, false, false, // left arm
                                          false, false, false, false, false, false, false, // right arm
                                          false, false, false, false, false, // left leg
                                          false, false, false, false, false  // right leg
            };
			
            inline bool engageLock = false;
            inline bool engageLockTR = false; // Target Reacquisition ( re-target after getting a kill when spraying ).
            inline int engageLockTTR = 700; // Time to Target Reacquisition in ms
		}

		namespace ShootAssist
		{
			inline bool enabled = false;

			namespace ShotDelay
			{
				inline float Value = 100.f;
			}	
			namespace MinShotFire
			
			{
				inline int value = 6;
			} // namespace MinShotFire

			namespace Hitchance
			{
				inline bool enabled = false;
				inline float value = 20;
			}
		}

		namespace AutoWall
		{
			inline bool enabled = false;
			inline float value = 10.0f;
		}

		namespace AimStep
		{
			inline bool enabled = false;
			inline float min = 25.0f;
			inline float max = 35.0f;
		}

		namespace RCS
		{
			inline bool enabled = false;
			inline bool always_on = false;
			inline float valueX = 2.0f;
			inline float valueY = 2.0f;
		}

		namespace AutoPistol
		{
			inline bool enabled = false;
		}

		namespace AutoShoot
		{
			inline bool enabled = false;
			inline bool velocityCheck = false;
			inline bool autoscope = false;
		}

		namespace AutoCrouch
		{
			inline bool enabled = false;
		}

		namespace AutoSlow
		{
			inline bool enabled = false;
		}

		namespace NoShoot
		{
			inline bool enabled = false;
		}

		namespace IgnoreJump
		{
			inline bool enabled = false;
		}

		namespace IgnoreEnemyJump
		{
			inline bool enabled = false;
		}

		namespace SmokeCheck
		{
			inline bool enabled = false;
		}

		namespace FlashCheck
		{
			inline bool enabled = false;
		}

		
		
		

		namespace Prediction
		{
			inline bool enabled = false;
		}

		namespace ScopeControl
		{
			inline bool enabled = false;
		}

		inline std::unordered_map<ItemDefinitionIndex, LegitWeapon_t, Util::IntHash<ItemDefinitionIndex>> weapons = {
                { ItemDefinitionIndex::INVALID, defaultSettings },
        };
	}
		namespace ProfileChanger
                {
                extern int coinID;
                extern int musicID;
                extern int compRank;
                extern int weaponStatus;
                extern int weaponRarity;
		extern int music;
		extern int rank;
		extern int coin;
                extern bool enabled;
		inline MusicType type = MusicType::CSGO;
                }

	namespace Ragebot
	{
			inline impactType impacttype = impactType::ITSME;
		namespace exploits
		{
                        inline ButtonCode_t doubletapKey;
                        inline bool doubletapToggle;
                        inline bool enabled;
		        inline bool doubletap;
 	                inline DtSpeed doubletapSpeed = DtSpeed::INSTANT;
		}

		inline float test;
		namespace quickpeek
		{
         		inline ButtonCode_t key;
			inline bool enabled;
		        inline ColorVar color = ImColor(255, 255, 255, 190 );
		}
		inline bool DoubleTap;
		inline float MinDamage = 50.f;
		inline bool enabled = false;
        	inline bool silent = false;
        	inline bool friendly = false;
		inline bool DoubleFire = false;

		inline DamagePrediction damagePrediction = DamagePrediction::damage;
		inline EnemySelectionType enemySelectionType = EnemySelectionType::CLosestToCrosshair;

		namespace AutoAim
		{
			inline bool enabled = false;
			inline bool desireBones[] = {true, true, true, true, true, true};
		}

		namespace AutoPistol
		{
			inline bool enabled = false;
		}

		namespace AutoShoot
		{
			inline bool enabled = false;
			inline bool velocityCheck = false;
			inline bool autoscope = false;
		}

		namespace AutoSlow
		{
			inline bool enabled = false;
		}

		namespace HitChance
		{
			inline bool enabled = false;
			inline float value = 20.f;
		}

		namespace HitChanceOverwrride 
		{
			inline bool enable = false;
			inline float value = 1.0f;
		}

		namespace ScopeControl
		{
			inline bool enabled = false;
		}

		namespace AutoCrouch
		{
			inline bool enable = false;
		}

		namespace backTrack
		{
			inline bool enabled = false;
			inline float time = 0.2f;
		}

		inline std::unordered_map<ItemDefinitionIndex, RageWeapon_t, Util::IntHash<ItemDefinitionIndex>> weapons = {
                { ItemDefinitionIndex::INVALID, ragedefault },
        };
	}



	namespace Triggerbot
	{
		inline bool enabled = false;
		inline ButtonCode_t key = ButtonCode_t::KEY_LALT;

		namespace Magnet
		{
			inline bool enabled = false;
		}

		namespace Filters
		{
			inline bool enemies = true;
			inline bool allies = false;
			inline bool walls = false;
			inline bool smokeCheck = false;
			inline bool flashCheck = false;
			inline bool head = true;
			inline bool chest = true;
			inline bool stomach = true;
			inline bool arms = true;
			inline bool legs = true;
		}

		namespace RandomDelay
		{
			inline bool enabled = false;
			inline int lowBound = 20; // in ms
			inline int highBound = 35;// in ms
			inline int lastRoll = 0;
		}
	}
    namespace SilentWalk
    {

	inline bool enabled;
	inline ButtonCode_t key = ButtonCode_t::KEY_Z;
    }
    namespace AntiAim
    {

            inline AntiAimType_X pitchtype = AntiAimType_X::STATIC_DOWN;
	inline bool lbyjitter;
	namespace airspin
		{
	inline bool enabled;
		}
	namespace SlowWalk 
		{
	inline float Speed;
	inline bool enabled;
	inline ButtonCode_t key;

		}
	namespace Type
		{
			inline AntiAimType antiaimType = AntiAimType::LegitAntiAim;
		}
        namespace AutoDisable
        {
            inline bool noEnemy = false;
            inline bool knifeHeld = false;
        }
        namespace RageAntiAim
        {
	    inline bool pitchJitter;
	    inline float offset = 180;
	    inline bool invertOnHurt = false;
            inline bool enable = false;
			inline ButtonCode_t InvertKey;
			inline bool inverted = false;
			inline float AntiAImPercent = 100.f;
			inline float JitterPercent = 30.f;
			inline bool atTheTarget = false;
			inline bool SendReal = false;
			inline RageAntiAimType Type = RageAntiAimType::DefaultRage;
        		inline LbyMode lbym = LbyMode::Normal;
	}
		namespace LegitAntiAim 
		{
			inline bool enable = false;
			inline bool OverWatchProof = true;
			inline ButtonCode_t InvertKey = ButtonCode_t::KEY_T;
			inline bool inverted = false;
			inline float RealPercentage = 30.f;
			inline float RealPercentageInCroutch = 30.f;
			inline LegitAAType legitAAtype = LegitAAType::OverWatchProof;
		}
		namespace ManualAntiAim
		{
			inline bool Enable = false;
			inline ButtonCode_t backButton = ButtonCode_t::KEY_X;
			inline ButtonCode_t RightButton = ButtonCode_t::KEY_C;
			inline ButtonCode_t LeftButton = ButtonCode_t::KEY_Z;
		}
        namespace Yaw
        {
            inline AntiAimRealType_Y typeReal = AntiAimRealType_Y::Static;
            inline AntiAimFakeType_y typeFake = AntiAimFakeType_y::Static;
        }
        namespace HeadEdge
        {
            inline bool enabled = false;
            inline float distance = 25.0f;
        }
        namespace LBYBreaker
        {
            inline bool enabled = false;
            inline float offset = 180.0f;
        }
		namespace FakeDuck
		{
			inline bool enabled = false;
			inline ButtonCode_t fakeDuckKey;
		}
  namespace randomLag
                {
                        inline bool enabled = false;
                }

	}

	namespace Resolver
	{
		inline bool rPitch;
		inline float Pitch;
		inline float resolveDelt = 0.5f;
		inline bool resolveAll = false;
		inline float goalFeetYaw;
		inline float EyeAngles;
		inline bool manual = false;
		inline bool forcebrute = false;
		inline bool resolveAllAP;
                inline resolverType resolverType = resolverType::NONE;

	}

	namespace ESP
	{
		inline bool indicators;
		inline bool drawback;
		inline bool showimpacts;
		inline bool KeyBinds = false;
	namespace keybi
	{
		inline int x;
		inline int y;
		inline int w;
		inline int h;
	}
	inline ColorVar manualAAColor;

	inline bool showDormant = true;
	inline ColorVar dormantColor = ImColor(255, 255, 255, 255);
	inline bool enabled = false;
        inline DrawingBackend backend = DrawingBackend::IMGUI;
		inline Filter filter = Filter::Enemies;
		inline ButtonCode_t key = ButtonCode_t::KEY_Z;
		inline TeamColorType teamColorType = TeamColorType::RELATIVE;
        inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
        inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
        inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
        inline HealthColorVar allyVisibleColor = ImColor(0, 255, 0, 255);
        inline HealthColorVar tColor = ImColor(255, 0, 0, 255);
        inline HealthColorVar tVisibleColor = ImColor(255, 255, 0, 255);
        inline HealthColorVar ctColor = ImColor(0, 0, 255, 255);
        inline HealthColorVar ctVisibleColor = ImColor(0, 255, 0, 255);
        inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
        inline ColorVar bombColor = ImColor(156, 39, 176, 255);
        inline ColorVar bombDefusingColor = ImColor(213, 0, 249, 255);
        inline ColorVar hostageColor = ImColor(121, 85, 72, 255);
        inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
        inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
        inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
        inline ColorVar fishColor = ImColor(255, 255, 255, 255);
        inline ColorVar smokeColor = ImColor(97, 97, 97, 255);
        inline ColorVar decoyColor = ImColor(2255, 152, 0, 255);
        inline ColorVar flashbangColor = ImColor(255, 235, 59, 255);
        inline ColorVar grenadeColor = ImColor(244, 67, 54, 255);
        inline ColorVar molotovColor = ImColor(205, 32, 31, 255);
        inline ColorVar mineColor = ImColor(205, 32, 31, 255);
        inline ColorVar chargeColor = ImColor(205, 32, 31, 255);
        inline ColorVar allyInfoColor = ImColor(255, 255, 255, 255);
        inline ColorVar enemyInfoColor = ImColor(255, 255, 255, 255);
           namespace Watermark
                {
                inline bool enabled = true;
                inline ColorVar color = ImColor(215, 5, 252, 255);
                }

		namespace FilterEnemy
		{
			namespace playerInfo
			{
				inline bool enabled = false;
			}
			namespace Glow
			{
				inline bool enabled = false;
            	inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
            	inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
            	inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
            	inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
            	inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
            	inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
           	 	inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
            	inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
			}
			namespace Skeleton
			{
				inline bool enabled = false;
            	inline ColorVar allyColor = ImColor(255, 255, 255, 255);
            	inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
			}
			namespace HeadDot
			{
				inline bool enabled = false;
				inline float size = 2.0f;
			}
			namespace BulletTracers
			{
				inline bool enabled = false;
			}
			namespace Boxes
			{
				inline bool enabled = false;
				inline BoxType type = BoxType::FRAME_2D;
			}

			namespace Chams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace HelthBar
			{
				inline bool enabled = false;
				inline BarType type = BarType::VERTICAL;
				inline BarColorType colorType = BarColorType::HEALTH_BASED;
			}

			namespace Tracers
			{
				inline bool enabled = false;
				inline TracerType type = TracerType::BOTTOM;
			}
		}

		namespace FilterAlise
		{
			namespace playerInfo
			{
				inline bool enabled = false;
			}

			namespace Glow
			{
				inline bool enabled = false;
            	inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
            	inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
            	inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
            	inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
            	inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
            	inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
           	 	inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
            	inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
			}
			namespace Skeleton
			{
				inline bool enabled = false;
            	inline ColorVar allyColor = ImColor(255, 255, 255, 255);
            	inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
			}
			namespace HeadDot
			{
				inline bool enabled = false;
				inline float size = 2.0f;
			}
			namespace BulletTracers
			{
				inline bool enabled = false;
			}
			namespace Boxes
			{
				inline bool enabled = false;
				inline BoxType type = BoxType::FRAME_2D;
			}

			namespace Chams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace HelthBar
			{
				inline bool enabled = false;
				inline BarType type = BarType::VERTICAL;
				inline BarColorType colorType = BarColorType::HEALTH_BASED;
			}

			namespace Tracers
			{
				inline bool enabled = false;
				inline TracerType type = TracerType::BOTTOM;
			}
		}

		namespace FilterLocalPlayer
		{
			namespace playerInfo
			{
				inline bool enabled = false;
			}
			
			namespace Glow
			{
				inline bool enabled = false;
            	inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
            	inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
            	inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
            	inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
            	inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
            	inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
           	 	inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
            	inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
			}
			namespace RealChams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace Chams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace Skeleton
			{
				inline bool enabled = false;
            	inline ColorVar allyColor = ImColor(255, 255, 255, 255);
            	inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
			}
			namespace HeadDot
			{
				inline bool enabled = false;
				inline float size = 2.0f;
			}
			namespace BulletTracers
			{
				inline bool enabled = false;
			}
			namespace Boxes
			{
				inline bool enabled = false;
				inline BoxType type = BoxType::FRAME_2D;
			}
			namespace HelthBar
			{
				inline bool enabled = false;
				inline BarType type = BarType::VERTICAL;
				inline BarColorType colorType = BarColorType::HEALTH_BASED;
			}

			namespace Tracers
			{
				inline bool enabled = false;
				inline TracerType type = TracerType::BOTTOM;
			}
		}

		namespace Glow
		{
			inline bool enabled = false;
            inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
            inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
            inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
            inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
            inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
            inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
            inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
            inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
		}

		namespace Filters
		{
			inline bool legit = false;
			inline bool visibilityCheck = false;
			inline bool smokeCheck = false;
			//inline bool flashCheck = false;
			inline bool enemies = false;
			inline bool allies = false;
			inline bool bomb = false;
			inline bool hostages = false;
			inline bool defusers = false;
			inline bool weapons = false;
			inline bool chickens = false;
			inline bool fishes = false;
			inline bool throwables = false;
			inline bool localplayer = false;
		}

		namespace Info
		{
			inline bool name = false;
			inline bool clan = false;
			inline bool steamId = false;
			inline bool rank = false;
			inline bool health = false;
			inline bool armor = false;
			inline bool weapon = false;
			inline bool scoped = false;
			inline bool reloading = false;
			inline bool flashed = false;
			inline bool planting = false;
			inline bool hasDefuser = false;
			inline bool defusing = false;
			inline bool grabbingHostage = false;
			inline bool rescuing = false;
			inline bool location = false;
			inline bool money = false;
		}

		namespace Skeleton
		{
			inline bool enabled = false;
            inline ColorVar allyColor = ImColor(255, 255, 255, 255);
            inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
		}

		namespace Boxes
		{
			inline bool enabled = false;
			inline BoxType type = BoxType::FRAME_2D;
		}

		namespace Sprite
		{
			inline bool enabled = false;
			inline SpriteType type = SpriteType::SPRITE_TUX;
		}

		namespace Bars
		{
			inline bool enabled = false;
			inline BarType type = BarType::HORIZONTAL;
			inline BarColorType colorType = BarColorType::HEALTH_BASED;
		}

		namespace Tracers
		{
			inline bool enabled = false;
			inline TracerType type = TracerType::BOTTOM;
		}

		namespace BulletTracers
		{
			inline bool enabled = false;
		}

		namespace Bomb
		{
			inline bool enabled = false;
		}
		namespace FOVCrosshair
		{
			inline bool enabled = false;
			inline bool filled = false;
			inline ColorVar color = ImColor(255, 0, 0, 255);
		}

		namespace Chams
		{
	
			inline bool enabled = false;
            inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
            inline HealthColorVar allyVisibleColor = ImColor(0, 255, 0, 255);
            inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
            inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
            inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
			inline HealthColorVar FakeColor = ImColor(124,145,25,225);

			inline ChamsType type = ChamsType::WHITEADDTIVE;

			namespace Arms
			{
				inline bool enabled = false;
				inline ColorVar color = ImColor(255, 255, 255, 255);
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}

			namespace Weapon
			{
				inline bool enabled = false;
				inline ColorVar color = ImColor( 255, 255, 255, 255 );
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
		}

		// sound esp
		namespace Sounds
		{
			inline bool enabled = false;
			inline int time = 1000;
		}

		namespace Hitmarker
		{
			inline bool enabled = false;
			inline bool enemies = false;
			inline bool allies = false;
			inline ColorVar color = ImColor(255, 0, 0, 255);
			inline int duration = 2000;
			inline int size = 16;
			inline int innerGap = 5;

			namespace Damage
			{
				inline bool enabled = false;
			}
			namespace Sounds {
				inline bool enabled = false;
				inline Sound sound = Sound::NONE;
			}
		}

		namespace HeadDot
		{
			inline bool enabled = false;
			inline float size = 2.0f;
		}

		namespace Spread
		{
			inline bool enabled = false; // show current spread
			inline bool spreadLimit = false; // show spreadLimit value
			inline ColorVar color = ImColor(15, 200, 45, 255);
			inline ColorVar spreadLimitColor = ImColor(20, 5, 150, 255);
		}

		namespace DangerZone
		{
			inline int drawDist = 2000;
			inline bool drawDistEnabled = false;
			inline bool upgrade = false;
			inline bool lootcrate = false;
			inline bool radarjammer = false;
			inline bool barrel = false;
			inline bool ammobox = false;
			inline bool safe = false;
			inline bool dronegun = false;
			inline bool drone = false;
			inline bool cash = false;
			inline bool tablet = false;
			inline bool healthshot = false;
			inline bool melee = false;
            inline ColorVar upgradeColor = ImColor(255, 0, 0, 255);
            inline ColorVar lootcrateColor = ImColor(255, 0, 0, 255);
            inline ColorVar radarjammerColor = ImColor(255, 0, 0, 255);
            inline ColorVar barrelColor = ImColor(255, 0, 0, 255);
            inline ColorVar ammoboxColor = ImColor(255, 0, 0, 255);
            inline ColorVar safeColor = ImColor(255, 0, 0, 255);
            inline ColorVar dronegunColor = ImColor(255, 0, 0, 255);
            inline ColorVar droneColor = ImColor(255, 0, 0, 255);
            inline ColorVar cashColor = ImColor(255, 0, 0, 255);
            inline ColorVar tabletColor = ImColor(255, 0, 0, 255);
            inline ColorVar healthshotColor = ImColor(255, 0, 0, 255);
            inline ColorVar meleeColor = ImColor(255, 0, 0, 255);
		}
	}

	namespace MaterialConfig {
		inline bool enabled = false;
		inline MaterialSystem_Config_t config = {};
	}

	namespace Dlights
	{
		inline bool enabled = false;
		inline float radius = 500.0f;
	}
	
	namespace Eventlog
	{
		inline bool showEnemies = false;
		inline bool showTeammates = false;
		inline bool showLocalplayer = false;
		inline float duration = 5000;
		inline float lines = 10;
		inline ColorVar color = ImColor( 255, 79, 56, 255 );
	}	

	namespace Spammer
	{
		inline SpammerType type = SpammerType::SPAMMER_NONE;
		inline bool say_team = false;

		namespace KillSpammer
		{
			inline bool enabled = false;
			inline bool sayTeam = false;
			inline std::vector<std::string> messages = {};
		}

		namespace RadioSpammer
		{
			inline bool enabled = false;
		}

		namespace NormalSpammer
		{
			inline std::vector<std::string> messages = {};
		}

		namespace PositionSpammer
		{
			inline int team = 1;
			inline bool showName = true;
			inline bool showWeapon = true;
			inline bool showRank = true;
			inline bool showWins = true;
			inline bool showHealth = true;
			inline bool showMoney = true;
			inline bool showLastplace = true;
		}
	}

	namespace BHop
	{
		inline bool enabled = false;

		namespace Chance
		{
			inline bool enabled = false;
			inline int value = 70;
		}

		namespace Hops
		{
			inline bool enabledMax = false;
			inline int Max = 7;
			inline bool enabledMin = false;
			inline int Min = 3;
		}
	}
namespace FakeWalk
{
inline bool enabled;
inline int speed;
inline ButtonCode_t key;
}

	namespace NoDuckCooldown
	{
		inline bool enabled = false;
	}

	namespace AutoStrafe
	{
		inline bool enabled = false;
		inline AutostrafeType type = AutostrafeType::AS_FORWARDS;
		inline bool silent = true;
	}

	namespace Noflash
	{
		inline bool enabled = false;
		inline float value = 160.0f; // maxalpha 0-255
	}

	namespace FOVChanger
	{
		inline bool enabled = false;
		inline bool viewmodelEnabled = false;
		inline float value = 100.0f;
		inline float viewmodelValue = 90.0f;
		inline bool ignoreScope = true;
	}

	namespace Radar
	{
		inline bool enabled = false;
		inline float zoom = 16.0f;
		inline bool enemies = false;
		inline bool allies = false;
		inline bool bomb = false;
		inline bool defuser = false;
		inline bool legit = false ;
		inline bool visibilityCheck = false;
		inline bool smokeCheck = false;
		inline TeamColorType teamColorType = TeamColorType::RELATIVE;
        inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
        inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
        inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
        inline HealthColorVar allyVisibleColor = ImColor(0, 255, 0, 255);
        inline HealthColorVar tColor = ImColor(255, 0, 0, 255);
        inline HealthColorVar tVisibleColor = ImColor(255, 255, 0, 255);
        inline HealthColorVar ctColor = ImColor(0, 0, 255, 255);
        inline HealthColorVar ctVisibleColor = ImColor(0, 255, 0, 255);
        inline ColorVar bombColor = ImColor(156, 39, 176, 255);
        inline ColorVar bombDefusingColor = ImColor(213, 0, 249, 255);
        inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
		inline float iconsScale = 4.5f;
		inline ImVec2 pos = ImVec2(0,0);

		namespace InGame
		{
			inline bool enabled = false;
		}
	}

	namespace Recoilcrosshair
	{
		inline bool enabled = false;
		inline bool showOnlyWhenShooting = false;
	}

	namespace RagdollGravity
	{
		inline bool enabled = false;
	}

	namespace Autoblock
	{
		inline bool enabled = false;
		inline ButtonCode_t key = ButtonCode_t::KEY_6;
	}

	namespace SkinOrModel
	{
		inline SkinAndModel skin_or_model = SkinAndModel::Model;
	}
	namespace Skinchanger
	{
	inline std::string PlayerModel = "models/player/custom_player/legacy/ctm_st6_varianti.mdl";
		namespace Skins
		{
			inline bool enabled = false;
			inline bool perTeam = false;
		}

		namespace Models
		{
			inline bool enabled = false;
		}

		inline std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsCT = {
                { ItemDefinitionIndex::WEAPON_AK47 /*WeaponID*/, { ItemDefinitionIndex::INVALID /*itemDefinitionIndex*/, 524 /*fallbackPaintKit*/, 0.0005f /*fallbackWear*/, -1 /*fallbackSeed*/, 1337/*fallbackStatTrak*/, -1/*fallbackEntityQuality*/, "TestTux"/*customName*/ } },
                { ItemDefinitionIndex::WEAPON_KNIFE, { ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, -1, -1, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_CT_SIDE, { ItemDefinitionIndex::GLOVE_SPECIALIST, 10006, 0.0005f, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_T_SIDE, { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, 10006, 0.0005f, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, { ItemDefinitionIndex::INVALID, 10006, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPORTY, { ItemDefinitionIndex::INVALID, 10018, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SLICK, { ItemDefinitionIndex::INVALID, 10013, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_MOTORCYCLE, { ItemDefinitionIndex::INVALID, 10024, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_LEATHER_WRAP, { ItemDefinitionIndex::INVALID, 10009, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPECIALIST, { ItemDefinitionIndex::INVALID, 10033, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, 1337, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_FLIP, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GUT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_PUSH, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_URSUS, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_STILETTO, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_CSS, { ItemDefinitionIndex::INVALID, -1, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GHOST, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                //{ ItemDefinitionIndex::WEAPON_KNIFEGG, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_USP_SILENCER, { ItemDefinitionIndex::INVALID, 2, 0.0005f, -1, -1, -1, ""} },
        };
		inline std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsT = {
                { ItemDefinitionIndex::WEAPON_AK47 /*WeaponID*/, { ItemDefinitionIndex::INVALID /*itemDefinitionIndex*/, 524 /*fallbackPaintKit*/, 0.0005f /*fallbackWear*/, -1 /*fallbackSeed*/, 1337/*fallbackStatTrak*/, -1/*fallbackEntityQuality*/, "TestTux"/*customName*/ } },
                { ItemDefinitionIndex::WEAPON_KNIFE_T, { ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, -1, -1, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_T_SIDE, { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, 10006, 0.0005f, -1, -1, -1, "" } },
                { ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND, { ItemDefinitionIndex::INVALID, 10006, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPORTY, { ItemDefinitionIndex::INVALID, 10018, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SLICK, { ItemDefinitionIndex::INVALID, 10013, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_MOTORCYCLE, { ItemDefinitionIndex::INVALID, 10024, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_LEATHER_WRAP, { ItemDefinitionIndex::INVALID, 10009, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::GLOVE_SPECIALIST, { ItemDefinitionIndex::INVALID, 10033, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, 1337, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BAYONET, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_FLIP, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GUT, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_PUSH, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_URSUS, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_STILETTO, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_CSS, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                { ItemDefinitionIndex::WEAPON_KNIFE_GHOST, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
                //{ ItemDefinitionIndex::WEAPON_KNIFEGG, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
        };
	}

	namespace ShowRanks
	{
		inline bool enabled = false;
	}
       namespace ShowKills
        {
                inline bool enabled = false;
        }


	namespace ShowSpectators
	{
		inline bool enabled = false;
	}

	namespace ClanTagChanger
	{
		inline char value[30] = {0};
		inline bool animation = false;
		inline int animationSpeed = 650;
		inline bool enabled = false;
		inline ClanTagType type = ClanTagType::STATIC;
	}

	namespace View
	{
		namespace NoAimPunch
		{
			inline bool enabled = false;
		}

		namespace NoViewPunch
		{
			inline bool enabled = false;
		}
	}

	namespace FakeLag
	{
		inline bool enabled = false;
		inline int value = 9;
		inline bool adaptive = false;
                inline ButtonCode_t ckey;

	}

	namespace AutoAccept
	{
		inline bool enabled = false;
	}

	namespace NoSky
	{
		inline bool enabled = false;
		inline ColorVar color = ImColor(0, 0, 0, 255);
	}

	namespace SkyBox
	{
		inline bool enabled = false;
		inline int skyBoxNumber = 0; // number in skyBoxNames
	}

	namespace ASUSWalls
	{
		inline bool enabled = false;
		inline ColorVar color = ImColor(255, 255, 255, 150);
	}

	namespace NoScopeBorder
	{
		inline bool enabled = false;
	}

	namespace SniperCrosshair
	{
		inline bool enabled = false;
	}

	namespace AutoDefuse
	{
		inline bool enabled = false;
		inline bool silent = false;
	}

	namespace NoSmoke
	{
		inline bool enabled = false;
		inline SmokeType type = SmokeType::NONE;
	}

	namespace ScreenshotCleaner
	{
		inline bool enabled = false;
	}

	namespace EdgeJump
	{
		inline bool enabled = false;
		inline ButtonCode_t key = ButtonCode_t::KEY_C;
	}

	namespace NameStealer
	{
		inline bool enabled = false;
		inline int team = 1;
	}

	namespace ThirdPerson
	{
		inline bool enabled = false;
		inline bool toggled = true;
		inline ButtonCode_t toggleThirdPerson = ButtonCode_t::KEY_LALT;
		inline float distance = 100.0f;
	}

	namespace JumpThrow
	{
		inline bool enabled = false;
		inline ButtonCode_t key = ButtonCode_t::KEY_T;
	}

	namespace NoFall
	{
		inline bool enabled = false;
	}

namespace SvCheats
{
inline bool enabled;

namespace gravity {
inline bool enabled;
inline int amount;
}

namespace aspect
{
inline bool enabled;
inline float var;
}

namespace bright
{
inline bool enabled;
}

namespace fog
{
inline bool enabled;
}

namespace impacts
{
inline bool enabled;
}


namespace viewmodel
{
inline bool enabled;
inline float fov;
inline float x;
inline float y;
inline float z;
}

namespace grenadetraj
{
inline bool enabled;
}

}


	namespace DisablePostProcessing
	{
		inline bool enabled = false;
	}

	namespace GrenadeHelper
	{
		inline std::vector<GrenadeInfo> grenadeInfos = {};
		inline bool enabled = false;
		inline bool onlyMatchingInfos = true;
		inline bool aimAssist = false;
		inline float aimStep = 5;
		inline float aimDistance = 75;
		inline float aimFov = 45;
        inline ColorVar aimDot = ImColor(10, 10, 200, 255);
        inline ColorVar aimLine = ImColor(200, 200, 200, 255);
        inline ColorVar infoHE = ImColor(7, 183, 7, 255);
        inline ColorVar infoMolotov = ImColor(236, 0, 0, 255);
        inline ColorVar infoSmoke = ImColor(172, 172, 172, 255);
        inline ColorVar infoFlash = ImColor(255, 255, 0, 255);
		inline std::string actMapName = {};
	}

	namespace GrenadePrediction
	{
		inline bool enabled = false;
		inline ColorVar color = ImColor( 255, 79, 56, 255 );

	}

	namespace TracerEffects
	{
		inline bool enabled = false;
		inline bool serverSide = false;
		inline TracerEffects_t effect = TracerEffects_t::TASER;
		inline int frequency = 1;

	}
	namespace AutoKnife
 	{
 		inline bool enabled = false;
 		inline bool onKey = true;

 		namespace Filters
 		{
 			inline bool enemies = true;
 			inline bool allies = false;
 		}
 	}

	namespace QuickSwitch
	{
		inline bool enabled;
	}

	namespace AngleIndicator
	{
		inline bool enabled = false;
	}
	namespace voterevealer
	{
		inline bool enabled;
	}
	namespace buybot
	{
	inline bool enabled = false;
	inline bool scout = false;
	inline bool autosniper = false;
	}
    namespace Debug
    {
        namespace AutoWall
        {
            inline bool debugView = false;
        }
		namespace AutoAim
		{
			inline bool drawTarget = false;
			inline Vector target = Vector(0);
		}
		namespace BoneMap
		{
			inline bool draw = false;
			inline bool justDrawDots = false;
			inline int modelID = 1253; // in econItemMap, not itemdefindex
		}
		namespace AnimLayers
		{
			inline bool draw = false;
		}
    }

	void LoadDefaultsOrSave(std::string path);
	void LoadConfig(std::string path);
	void LoadSettings();
	void DeleteConfig(std::string path);
	void SaveGrenadeInfo(std::string path);
	void LoadGrenadeInfo(std::string path);
}
