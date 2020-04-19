#include "antiaim.h"

#include "legitbot.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"

QAngle AntiAim::realAngle;
QAngle AntiAim::fakeAngle;

float AntiAim::GetMaxDelta( CCSGOAnimState *animState ) {

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
static void RageAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool bSend){

    //float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

    bSend ? angle.y -= 190.0f : angle.y -= 180.0f ;

    if(bSend) {
        AntiAim::fakeAngle.y = angle.y;
        AntiAim::fakeAngle.x = 89.0f;
    }
    else
    {
        AntiAim::realAngle.y = angle.y;
        AntiAim::realAngle.x = 89.0f;
    }
}

static QAngle LegitAntiAim(CUserCmd* cmd ,C_BasePlayer *const localplayer, QAngle angle, bool bSend){

    //float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    //bSend ? angle.y -=  maxDelta/2.0f : angle.y += maxDelta/2.0f ;
    bSend ? angle.x = angle.x : angle.x = 89.0f ;

    if(bSend) {
        AntiAim::fakeAngle = CreateMove::lastTickViewAngles;
        return AntiAim::fakeAngle;
    }
    else
    {
        angle.y -= 180.0f;
        AntiAim::realAngle.y = angle.y; 
        AntiAim::realAngle.x = angle.x;
        return AntiAim::realAngle;
       
    }
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    if (!Settings::AntiAim::RageAntiAim::enable && !Settings::AntiAim::LegitAntiAim::enable && !Settings::AntiAim::LBYBreaker::enabled)
    {    
        AntiAim::realAngle = AntiAim::fakeAngle = CreateMove::lastTickViewAngles;
        return;
    }

    if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
        return;

    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    

    QAngle angle = cmd->viewangles;

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
            return;
    }

    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 && *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
        return;

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;

    // Knife
    if (Settings::AntiAim::AutoDisable::knifeHeld && localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return;

    if (Settings::AntiAim::AutoDisable::noEnemy && localplayer->GetAlive() && !HasViableEnemy())
        return;

    QAngle edge_angle = angle;
    bool edging_head = Settings::AntiAim::HeadEdge::enabled && GetBestHeadAngle(edge_angle);

    static bool bSend = true;
    bSend = !bSend;

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

    /*
    ** We have to state rage anti aim and legit anti aim 
    ** legit is for legit configs and rage is desined for hvh moments
    */
    if(Settings::AntiAim::LegitAntiAim::enable && !needToFlick) {
       angle =  LegitAntiAim(cmd, localplayer, angle, bSend);
        CreateMove::sendPacket = bSend;
    }
    else if (Settings::AntiAim::RageAntiAim::enable && !needToFlick)
    {
        RageAntiAim(localplayer, angle, bSend);
        CreateMove::sendPacket = bSend;
        if (Settings::AntiAim::HeadEdge::enabled && edging_head && !bSend)
            {angle.y = edge_angle.y;}
    }


    if (!ValveDSCheck::forceUT && (*csGameRules) && (*csGameRules)->IsValveDS())
    {
        if (Settings::AntiAim::Pitch::type >= AntiAimType_X::STATIC_UP_FAKE)
            Settings::AntiAim::Pitch::type = AntiAimType_X::STATIC_UP;
    }

    // if (Settings::AntiAim::Pitch::enabled)
    //     DoAntiAimX(angle, bSend, should_clamp);

    // if( should_clamp ){
    //     Math::NormalizeAngles(angle);
    //     Math::ClampAngles(angle);
    // }

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
