#include "antiaim.h"

#include "../Hacks/legitbot.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"

#ifndef GetPercentVal
    #define GetPercentVal(val, percent) ( val * (percent/100) )
#endif

#ifndef LessThan
    #define LessThan(x, y) (x < x)
#endif

#ifndef IsEqual
    #define IsEqual(x, y) (x == x)
#endif

QAngle AntiAim::LastTickViewAngle;

static float buttonToggle = false;

static bool bSend = true,
            Bpressed = false;


float AntiAim::GetMaxDelta( CCSGOAnimState *animState ) 
{

    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }

    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;

    return delta - 0.5f;
}

static float Distance(Vector a, Vector b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

static bool GetBestHeadAngle(QAngle& angle)
{
    if ( !Ragebot::LockedEnemy )
        return false;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    Vector position = localplayer->GetVecOrigin() + localplayer->GetVecViewOffset();

    float closest_distance = 100.0f;

    float radius = Settings::AntiAim::HeadEdge::distance + 0.1f;
    static float step = M_PI * 2.0 / 8;

    for (float a = 0; a < (M_PI * 2.0); a += step)
    {
        Vector location(radius * cos(a) + position.x, radius * sin(a) + position.y, position.z);

        Ray_t ray;
        trace_t tr;
        ray.Init(position, location);
        CTraceFilter traceFilter;
        traceFilter.pSkip = localplayer;
        trace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);
        
        Vector localPos = localplayer->GetAbsOrigin();
        Vector enemyPos = Ragebot::LockedEnemy->GetAbsOrigin();
        float distance = Distance(position, tr.endpos);

        if (distance < closest_distance)
        {
            closest_distance = distance;
            // Enemy Must be in left
            if (  LessThan(localPos.x, enemyPos.x) && LessThan(localPos.z, enemyPos.z) )
            {    
                //angle.y = RAD2DEG(a);
                angle.y += 90.f;
            }
            else if ( IsEqual(localPos.x, enemyPos.x) && IsEqual(localPos.z, enemyPos.z))
            {
                angle.y = Math::CalcAngle(localPos, enemyPos).y + 180.f;
            }
            else 
            {
                angle.y -= 90.f;  
            }
            
        }
    }

    return true;
}

static bool HasViableEnemy()
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    for (int i = 1; i < engine->GetMaxClients(); ++i)
    {
        C_BasePlayer* entity = (C_BasePlayer*) entityList->GetClientEntity(i);

        if (!entity
            || entity == localplayer
            || entity->GetDormant()
            || !entity->GetAlive()
            || entity->GetImmune())
            continue;

        if( !Legitbot::friends.empty() ) // check for friends, if any
        {
            IEngineClient::player_info_t entityInformation;
            engine->GetPlayerInfo(i, &entityInformation);

            if (std::find(Legitbot::friends.begin(), Legitbot::friends.end(), entityInformation.xuid) != Legitbot::friends.end())
                continue;
        }

        if (Settings::Legitbot::friendly || !Entity::IsTeamMate(entity, localplayer))
            return true;
    }

    return false;
}

// Funtion for Rage Anti AIm
static void DoAntiAimY(C_BasePlayer *const localplayer, QAngle& angle, const bool& bSend)
{
    
    using namespace Settings::AntiAim::RageAntiAim;
    AntiAimRealType_Y Real_aa_type = Settings::AntiAim::Yaw::typeReal;
    AntiAimFakeType_y Fake_aa_type = Settings::AntiAim::Yaw::typeFake;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
   

    /* Button Function for invert the fake*/
    if ( inputSystem->IsButtonDown(InvertKey) && buttonToggle == false )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle == true)
		buttonToggle = false;
    
    if( bSend )
    {
        static bool bFlip = false;
        switch (Fake_aa_type)
        {
            case AntiAimFakeType_y::Static:
                angle.y = inverted ? AntiAim::realAngle.y - GetPercentVal(maxDelta, FakePercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, FakePercent);
                break;
            case AntiAimFakeType_y::Jitter:
                bFlip = !bFlip;
                angle.y = bFlip ? AntiAim::realAngle.y - GetPercentVal(maxDelta, FakePercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, FakePercent);
                break;
            default:
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    } 
    else
    {
        static bool bFlip = false;
        switch (Real_aa_type)
        {
            case AntiAimRealType_Y::Static :
                angle.y += 180.f;
                break;
            case AntiAimRealType_Y::Jitter:
                bFlip = !bFlip;
                angle.y = bFlip ? angle.y + GetPercentVal(180.f, RealJitterPercent) : angle.y - GetPercentVal(180.f, RealJitterPercent);
                break;
            default:
                break;
        }
        AntiAim::realAngle.y = angle.y;
    }
}

// Function to set the pitch angle
static void DoAntiAimX(QAngle& angle, bool& bsend)
{
    AntiAim::realAngle.x = AntiAim::fakeAngle.x = angle.x = 89.0f;
}

// Function For Legit AntiAim
static void DoLegitAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool &bSend)
{
    using namespace Settings::AntiAim::LegitAntiAim;
    
    if ( inputSystem->IsButtonDown(InvertKey) && buttonToggle == false )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle == true)
		buttonToggle = false;
    
    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

    /*
    * Actual area where we are settings the fake and real angle 
    * area is the variable which is responcible for changing angle so area is the vital variable
    * AntiAim::realangle and AntiAim::fakeangle is are set to determine the angle in thirdperson mode
    */
    if(!Settings::AntiAim::LegitAntiAim::inverted)
    {
        if (bSend)
        {
            AntiAim::fakeAngle = angle;        
        }
        else
        {
            angle.y -= (maxDelta / 1.2f);
            AntiAim::realAngle = angle;
        }
        
    }
    else
    {
        if ( bSend )
        {
           AntiAim::fakeAngle = angle;
        }
        else 
        {
            angle.y += (maxDelta / 1.2f);
            AntiAim::realAngle = angle;
        } 
    } 
        
}


/*
** Settings Manual Anti AIm 
** Simply What we do is change the value of YAxis for in Settings::AntiAIm::Yaw::real
** Basically THis si work as toggle
*/
static bool DoManuaAntiAim(C_BasePlayer* localplayer, const bool& bSend, QAngle& angle)
{

    using namespace AntiAim::ManualAntiAim;
    using namespace Settings::AntiAim::RageAntiAim;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    static bool bFlip = false;

    if (!Settings::AntiAim::ManualAntiAim::Enable)
        return false;

    // Manual anti aim set to back
    if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !Bpressed )
	{
		alignBack = !alignBack;
		alignRight = false;
        alignLeft = false;
	}
	else if ( !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton ) && Bpressed && alignBack)
	{
        Bpressed = false;
    }		

    //END BACK MANUAL

    //SETTING MANUAL FOR RIGHT
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !Bpressed)
	{
		alignBack = false;
		alignRight = !alignRight;
        alignLeft = false;
	}
	else if ( !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::RightButton ) && Bpressed && alignRight)
	{
        Bpressed = false;
    }		

    //END MANUAL RIGHT

    // SETTINGS MANUAL ANTIAIM FOR LEFT
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !Bpressed )
	{
		alignBack = false;
		alignRight = false;
        alignLeft = !alignLeft;
	}
	else if ( !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::LeftButton ) && Bpressed && alignLeft)
	{  
        Bpressed = false;
    }		
    
    AntiAimFakeType_y Fake_aa_type = Settings::AntiAim::Yaw::typeFake;
    // Applying ManualAntiAIm
    if ( !bSend )
    {
        if ( alignBack )
            angle.y += 180.f;
        else if (alignLeft)
            angle.y += 90.f;
        else if (alignRight)
            angle.y -= 90.f;

        AntiAim::realAngle.y = angle.y;
    }
    else
    {
        switch (Fake_aa_type)
        {
            case AntiAimFakeType_y::Static:
                angle.y = inverted ? AntiAim::realAngle.y - GetPercentVal(maxDelta, FakePercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, FakePercent);
                break;
            case AntiAimFakeType_y::Jitter:
                bFlip = !bFlip;
                angle.y = bFlip ? AntiAim::realAngle.y - GetPercentVal(maxDelta, FakePercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, FakePercent);
                break;
            default:
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    }

    return (alignRight || alignLeft || alignBack);
}


void AntiAim::CreateMove(CUserCmd* cmd)
{
    
    QAngle angle = cmd->viewangles;
    
    if (!Settings::AntiAim::RageAntiAim::enable && !Settings::AntiAim::LBYBreaker::enabled && !Settings::AntiAim::LegitAntiAim::enable && !Settings::AntiAim::ManualAntiAim::Enable)
    {
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
        return;
    }    

    if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
        return;

    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    
    if (!localplayer || !localplayer->GetAlive())
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
        return;

    if (activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeWeapon;

        if (csGrenade->GetThrowTime() > 0.f)
        {
            AntiAim::realAngle = AntiAim::fakeAngle = angle;
            return;
        }
    }

    

    if ( cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || ( cmd->buttons & IN_ATTACK2 && ( *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_KNIFE ) ) && !Ragebot::coacking == true)
    {
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
        return;
    }

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
    {
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
        return;
    }    

    // Knife
    
    if (Settings::AntiAim::AutoDisable::knifeHeld && localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
    {
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
        return;
    }
    if (Settings::AntiAim::AutoDisable::noEnemy && localplayer->GetAlive() && !HasViableEnemy())
    {
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
        return;
    }

    
    bSend = !bSend;
    bool should_clamp = true;
    bool needToFlick = false;
    static bool lbyBreak = false;
    static float lastCheck;
    float vel2D = localplayer->GetVelocity().Length2D();//localplayer->GetAnimState()->verticalVelocity + localplayer->GetAnimState()->horizontalVelocity;

    if( Settings::AntiAim::LBYBreaker::enabled ){

        if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
            lbyBreak = false;
            lastCheck = globalVars->curtime;
        } else {
            if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
                angle.y -= Settings::AntiAim::LBYBreaker::offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                angle.y -= Settings::AntiAim::LBYBreaker::offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            }
        }
    }
    else if (Settings::AntiAim::RageAntiAim::enable && !needToFlick) // responsible for reage anti aim or varity of anti aims .. 
    {
        
        
        
        // Check for manual antiaim
        if ( !DoManuaAntiAim(localplayer, bSend, angle) )
        {
            // Look toword the target that aimbot locked on
            if (Settings::AntiAim::RageAntiAim::atTheTarget && Ragebot::LockedEnemy)
            {
                angle.y = Math::CalcAngle(localplayer->GetAbsOrigin(), Ragebot::LockedEnemy->GetAbsOrigin()).y + 180.f;
            }
            
            DoAntiAimY(localplayer, angle, bSend);
            
            QAngle edge_angle = angle;
            bool edging_head = Settings::AntiAim::HeadEdge::enabled && GetBestHeadAngle(edge_angle);
            
            if (edging_head && !bSend)
                AntiAim::realAngle.y = angle.y = edge_angle.y;
        }
            
        DoAntiAimX(angle, bSend);

        CreateMove::sendPacket = bSend;
    }
    else if (Settings::AntiAim::LegitAntiAim::enable && !needToFlick) // Responsible for legit anti aim activated when the legit anti aim is enabled
    {
        DoLegitAntiAim(localplayer, angle, bSend);
        CreateMove::sendPacket = bSend;
    }    
    
    
    if( should_clamp ){
        Math::NormalizeAngles(angle);
        Math::ClampAngles(angle);
    }

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
