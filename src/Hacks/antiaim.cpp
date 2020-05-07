#include "antiaim.h"

#include "../Hacks/legitbot.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"


QAngle AntiAim::LastTickViewAngle;
static bool bSend = true;

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
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    Vector position = localplayer->GetVecOrigin() + localplayer->GetVecViewOffset();

    float closest_distance = 100.0f;

    float radius = Settings::AntiAim::HeadEdge::distance + 0.1f;
    float step = M_PI * 2.0 / 8;

    for (float a = 0; a < (M_PI * 2.0); a += step)
    {
        Vector location(radius * cos(a) + position.x, radius * sin(a) + position.y, position.z);

        Ray_t ray;
        trace_t tr;
        ray.Init(position, location);
        CTraceFilter traceFilter;
        traceFilter.pSkip = localplayer;
        trace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);

        float distance = Distance(position, tr.endpos);

        if (distance < closest_distance)
        {
            closest_distance = distance;
            angle.y = RAD2DEG(a);
        }
    }

    return closest_distance < Settings::AntiAim::HeadEdge::distance;
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
static void DoAntiAimY(C_BasePlayer *const localplayer, QAngle& angle, bool bSend)
{
    AntiAimType_Y Fake_aa_type = Settings::AntiAim::Yaw::typeFake;
    AntiAimType_Y Real_aa_type = Settings::AntiAim::Yaw::typeReal;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    static bool bFlip = false;
    //float lby = *localplayer->GetLowerBodyYawTarget();
    
    if( bSend ){
        switch (Fake_aa_type)
        {
            case AntiAimType_Y::NONE:
                AntiAim::fakeAngle.y = AntiAim::realAngle.y;
                break;
            case AntiAimType_Y::MAX_DELTA_LEFT:
                angle.y = AntiAim::realAngle.y - (maxDelta);
                break;
            case AntiAimType_Y::MAX_DELTA_RIGHT:
                angle.y = AntiAim::realAngle.y + (maxDelta);
                break;
            case AntiAimType_Y::MAX_DELTA_FLIPPER:
                bFlip = !bFlip;
                angle.y = bFlip ? AntiAim::realAngle.y - maxDelta : AntiAim::realAngle.y + maxDelta;
                break;
            case AntiAimType_Y::MAX_DELTA_LBY_AVOID:
                break;
            default:
                break;
        }

        AntiAim::fakeAngle.y = angle.y;
    } else {
        switch (Real_aa_type)
        {
            case AntiAimType_Y::NONE:
                AntiAim::realAngle = angle;
                break;
            case AntiAimType_Y::MAX_DELTA_LEFT:
                angle.y -= 90.f;
                break;
            case AntiAimType_Y::MAX_DELTA_RIGHT:
                angle.y += 90.f;
                break;
            case AntiAimType_Y::MAX_DELTA_FLIPPER:
                bFlip = !bFlip;
                angle.y = bFlip ? angle.y - maxDelta : angle.y + maxDelta;
                break;
            case AntiAimType_Y::MAX_DELTA_LBY_AVOID:
                angle.y += 180.f;
                break;
            default:
                break;
        }
        AntiAim::realAngle.y = angle.y;
    }
}

// Function to set the pitch angle
static void DoAntiAimX(QAngle& angle, bool bFlip, bool& clamp)
{
    static float pDance = 0.0f;
    AntiAimType_X aa_type = Settings::AntiAim::Pitch::type;

    switch (aa_type)
    {
        case AntiAimType_X::STATIC_UP:
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = -89.0f;
            break;
        case AntiAimType_X::STATIC_DOWN:
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 89.0f;
            break;
        case AntiAimType_X::DANCE:
            pDance += 45.0f;
            if (pDance > 100)
                pDance = 0.0f;
            else if (pDance > 75.f)
                AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = -89.f;
            else if (pDance < 75.f)
                AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 89.f;
            break;
        case AntiAimType_X::FRONT:
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 0.0f;
            break;
        case AntiAimType_X::STATIC_UP_FAKE:
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = bFlip ? 89.0f : -89.0f;
            break;
        case AntiAimType_X::STATIC_DOWN_FAKE:
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = bFlip ? -89.0f : 89.0f;
            break;
        case AntiAimType_X::LISP_DOWN:
            clamp = false;
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 1800089.0f;
            break;
        case AntiAimType_X::ANGEL_DOWN:
            clamp = false;
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 36000088.0f;
            break;
        case AntiAimType_X::ANGEL_UP:
            clamp = false;
            AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 35999912.0f;
            break;
        default:
            break;
    }
}

// Function For Legit AntiAim
static void DoLegitAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool &bSend)
{
    if (inputSystem->IsButtonDown(Settings::AntiAim::LegitAntiAim::InvertKey))
        Settings::AntiAim::LegitAntiAim::inverted = !Settings::AntiAim::LegitAntiAim::inverted;
        
    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

    /*
    * Actual area where we are settings the fake and real angle 
    * area is the variable which is responcible for changing angle so area is the vital variable
    * AntiAim::realangle and AntiAim::fakeangle is are set to determine the angle in thirdperson mode
    */
    if(!Settings::AntiAim::LegitAntiAim::inverted)
        angle.y = bSend ? AntiAim::fakeAngle.y = angle.y + (maxDelta / 2.f) : AntiAim::realAngle.y = angle.y - (maxDelta / 2.f);        
    else 
        angle.y = bSend ? AntiAim::fakeAngle.y = angle.y - (maxDelta / 2.f) : AntiAim::realAngle.y = angle.y + (maxDelta / 2.f);
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    if (!Settings::AntiAim::Yaw::enabled && !Settings::AntiAim::Pitch::enabled && !Settings::AntiAim::LBYBreaker::enabled && !Settings::AntiAim::LegitAntiAim::enable)
        return;

    if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
        return;

    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    
    QAngle angle = cmd->viewangles;

    AntiAim::fakeAngle = CreateMove::lastTickViewAngles;
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

    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 && *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER) && !Ragebot::coacking == true)
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

    QAngle edge_angle = angle;
    bool edging_head = Settings::AntiAim::HeadEdge::enabled && GetBestHeadAngle(edge_angle);

    
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

    else if (Settings::AntiAim::Yaw::enabled && !needToFlick) // responsible for reage anti aim or varity of anti aims .. 
    {
        DoAntiAimY(localplayer, angle, bSend);

        CreateMove::sendPacket = bSend;
        if (Settings::AntiAim::HeadEdge::enabled && edging_head && !bSend)
            AntiAim::realAngle.y = angle.y = edge_angle.y;
    }
    /*else if (Settings::AntiAim::LegitAntiAim::enable && !needToFlick) // Responsible for legit anti aim activated when the legit anti aim is enabled
    {
        DoLegitAntiAim(localplayer, angle, bSend);
        CreateMove::sendPacket = bSend;
    }*/

    if (!ValveDSCheck::forceUT && (*csGameRules) && (*csGameRules)->IsValveDS())
    {
        if (Settings::AntiAim::Pitch::type >= AntiAimType_X::STATIC_UP_FAKE)
            Settings::AntiAim::Pitch::type = AntiAimType_X::STATIC_UP;
    }

    if (Settings::AntiAim::Pitch::enabled)
        DoAntiAimX(angle, bSend, should_clamp);

    if( should_clamp ){
        Math::NormalizeAngles(angle);
        Math::ClampAngles(angle);
    }

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
