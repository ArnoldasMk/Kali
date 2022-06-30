#include "antiaim.h"
#include "legitbot.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"

QAngle AntiAim::realAngle;
QAngle AntiAim::fakeAngle;

bool dFlip = false;
float pDance;

AntiAimYaw yType = AntiAimYaw::NONE;
AntiAimPitch xType = AntiAimPitch::NONE;
AntiAimDesync dType = AntiAimDesync::NONE;

int yJitterLeft;
int yJitterRight;
int yJitterLeftMax;
int yJitterRightMax;
int ySpinFactor;

int xCustom;

int dJitterLeft;
int dJitterRight;
int dJitterLeftMax;
int dJitterRightMax;
int dOffset;

bool crouch; // crouch in air
bool canDoZ = false;

float AntiAim::GetMaxDelta(CCSGOAnimState *animState) {
    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));
    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));
    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;
    if (animState->duckProgress > 0) {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }
    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;
    return delta - 0.5f;
}

void AntiAim::CreateMove(CUserCmd *cmd)
{
    if (!Settings::AntiAim::enabled)
        return;

    if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
        return;

    if (!engine->IsInGame())
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

    static bool bPressed;
    if (inputSystem->IsButtonDown(Settings::AntiAim::dFlipKey) && !bPressed) {
        dFlip = !dFlip;
        bPressed = true;
    } else if (inputSystem->IsButtonDown(Settings::AntiAim::dFlipKey) && bPressed)
        bPressed = false;

    if (inputSystem->IsButtonDown(Settings::AntiAim::Manual::right))
        Settings::AntiAim::Manual::direction = 3; // right
    else if (inputSystem->IsButtonDown(Settings::AntiAim::Manual::left))
        Settings::AntiAim::Manual::direction = 1; // left
    else if (inputSystem->IsButtonDown(Settings::AntiAim::Manual::back))
        Settings::AntiAim::Manual::direction = 2; // back

    static bool bSend = true;
    bSend = !bSend;
    if (localplayer->GetVelocity().Length() <= 0.0f)
    {
        canDoZ = true;

        yType = Settings::AntiAim::Stand::Yaw::type;
        dType = Settings::AntiAim::Stand::Desync::type;
        xType = Settings::AntiAim::Stand::Pitch::type;

        yJitterLeft = Settings::AntiAim::Stand::Yaw::jitterLeft;
        yJitterRight = Settings::AntiAim::Stand::Yaw::jitterRight;
        yJitterLeftMax = Settings::AntiAim::Stand::Yaw::jitterLeftMax;
        yJitterRightMax = Settings::AntiAim::Stand::Yaw::jitterRightMax;
        ySpinFactor = Settings::AntiAim::Stand::Yaw::spinFactor;

        xCustom = Settings::AntiAim::Stand::Pitch::custom;

        dOffset = Settings::AntiAim::Stand::Desync::offset;
        dJitterLeft = Settings::AntiAim::Stand::Desync::jitterLeft;
        dJitterLeftMax = Settings::AntiAim::Stand::Desync::jitterLeftMax;
        dJitterRight = Settings::AntiAim::Stand::Desync::jitterRight;
        dJitterRightMax = Settings::AntiAim::Stand::Desync::jitterRightMax;

        crouch = false;
    } else if (inputSystem->IsButtonDown(Settings::SlowWalk::key)) {
        canDoZ = false;

        yType = Settings::AntiAim::SlowWalk::Yaw::type;
        dType = Settings::AntiAim::SlowWalk::Desync::type;
        xType = Settings::AntiAim::SlowWalk::Pitch::type;
        
        yJitterLeft = Settings::AntiAim::SlowWalk::Yaw::jitterLeft;
        yJitterRight = Settings::AntiAim::SlowWalk::Yaw::jitterRight;
        yJitterLeftMax = Settings::AntiAim::SlowWalk::Yaw::jitterLeftMax;
        yJitterRightMax = Settings::AntiAim::SlowWalk::Yaw::jitterRightMax;
        ySpinFactor = Settings::AntiAim::SlowWalk::Yaw::spinFactor;

        xCustom = Settings::AntiAim::SlowWalk::Pitch::custom;

        dOffset = Settings::AntiAim::SlowWalk::Desync::offset;
        dJitterLeft = Settings::AntiAim::SlowWalk::Desync::jitterLeft;
        dJitterLeftMax = Settings::AntiAim::SlowWalk::Desync::jitterLeftMax;
        dJitterRight = Settings::AntiAim::SlowWalk::Desync::jitterRight;
        dJitterRightMax = Settings::AntiAim::SlowWalk::Desync::jitterRightMax;

        crouch = false;
    } else if (!(localplayer->GetFlags() & FL_ONGROUND)) {
        canDoZ = false;

        yType = Settings::AntiAim::Air::Yaw::type;
        dType = Settings::AntiAim::Air::Desync::type;
        xType = Settings::AntiAim::Air::Pitch::type;
        
        yJitterLeft = Settings::AntiAim::Air::Yaw::jitterLeft;
        yJitterRight = Settings::AntiAim::Air::Yaw::jitterRight;
        yJitterLeftMax = Settings::AntiAim::Air::Yaw::jitterLeftMax;
        yJitterRightMax = Settings::AntiAim::Air::Yaw::jitterRightMax;
        ySpinFactor = Settings::AntiAim::Air::Yaw::spinFactor;

        xCustom = Settings::AntiAim::Air::Pitch::custom;

        dOffset = Settings::AntiAim::Air::Desync::offset;
        dJitterLeft = Settings::AntiAim::Air::Desync::jitterLeft;
        dJitterLeftMax = Settings::AntiAim::Air::Desync::jitterLeftMax;
        dJitterRight = Settings::AntiAim::Air::Desync::jitterRight;
        dJitterRightMax = Settings::AntiAim::Air::Desync::jitterRightMax;

        crouch = Settings::AntiAim::Air::crouch;
    } else {
        canDoZ = false;

        yType = Settings::AntiAim::Movement::Yaw::type;
        dType = Settings::AntiAim::Movement::Desync::type;
        xType = Settings::AntiAim::Movement::Pitch::type;
        
        yJitterLeft = Settings::AntiAim::Movement::Yaw::jitterLeft;
        yJitterRight = Settings::AntiAim::Movement::Yaw::jitterRight;
        yJitterLeftMax = Settings::AntiAim::Movement::Yaw::jitterLeftMax;
        yJitterRightMax = Settings::AntiAim::Movement::Yaw::jitterRightMax;
        ySpinFactor = Settings::AntiAim::Movement::Yaw::spinFactor;

        xCustom = Settings::AntiAim::Movement::Pitch::custom;

        dOffset = Settings::AntiAim::Movement::Desync::offset;
        dJitterLeft = Settings::AntiAim::Movement::Desync::jitterLeft;
        dJitterLeftMax = Settings::AntiAim::Movement::Desync::jitterLeftMax;
        dJitterRight = Settings::AntiAim::Movement::Desync::jitterRight;
        dJitterRightMax = Settings::AntiAim::Movement::Desync::jitterRightMax;

        crouch = false;
    }

    if ((Settings::FakeLag::enabled || (inputSystem->IsButtonDown(Settings::SlowWalk::key) && Settings::SlowWalk::mode == SlowMode::SAFETY)) ? !CreateMove::sendPacket : !bSend) {
        static bool flip = false;
        flip = !flip;
        if (Settings::AntiAim::ExperimentalZFLIP && canDoZ)
            angle.z = !flip ? 45.f : -45.f; // meme
        else {
            switch (dType)
            {
                case AntiAimDesync::RANDOMJITTER:
                    angle.y += flip ? Math::float_rand(dJitterRight, dJitterRightMax) :
                                      -Math::float_rand(dJitterLeft, dJitterLeftMax); 
                    break;

                case AntiAimDesync::JITTER:
                    angle.y += flip ? dJitterRight : -dJitterLeft;
                    break;

                case AntiAimDesync::STATIC:
                    angle.y += dFlip ? dOffset : -dOffset;
                    break;
                default: break;
            }
        }
    } else {
        static bool f2 = true;
        f2 = !f2;
        switch (yType)
        {
            case AntiAimYaw::SPIN:
                angle.y = fmodf(globalVars->curtime * ySpinFactor, 360.0);
                break;

            case AntiAimYaw::RANDOMJITTER:
                angle.y += f2 ? Math::float_rand(yJitterRight, yJitterRightMax) :
                                                         -Math::float_rand(yJitterLeft, yJitterLeftMax); 
                break;

            case AntiAimYaw::JITTER:
                angle.y += f2 ? yJitterRight : -yJitterLeft;
                break;

            default: break;
        }
    }
    
    if (Settings::AntiAim::Manual::enabled)
    {
        switch (Settings::AntiAim::Manual::direction)
        {
            case 1:
                angle.y += 90.f;
                break;
            case 3:
                angle.y -= 90.f;
                break;
            case 2:
                angle.y -= 180.f;
                break;
            default: break;
        }
    }
    
    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);
    switch (xType)
    {
        case AntiAimPitch::UP:
            angle.x = 89.f;
            break;
        case AntiAimPitch::DOWN:
            angle.x = -89.f;
            break;
        case AntiAimPitch::DANCE:
            pDance += 45.0f;
            if (pDance > 100)
                pDance = 0.0f;
            else if (pDance > 75.f)
                angle.x = -89.f;
            else if (pDance < 75.f)
                angle.x = 89.f;
            break;
        case AntiAimPitch::CUSTOM:
            angle.x = xCustom;
            break;
        default:
            break;
    }

    if (!Settings::FakeLag::enabled && !(inputSystem->IsButtonDown(Settings::SlowWalk::key) && Settings::SlowWalk::mode == SlowMode::SAFETY))
        CreateMove::sendPacket = bSend;

    if (crouch)
        cmd->buttons |= IN_DUCK;

    if (CreateMove::sendPacket)
        AntiAim::realAngle = CreateMove::lastTickViewAngles = angle;
    else
        AntiAim::fakeAngle = angle;

    cmd->viewangles = angle;
    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}

void AntiAim::FrameStageNotify(ClientFrameStage_t stage)
{
   
}

void AntiAim::OverrideView(CViewSetup *pSetup)
{
    
}
void AntiAim::FireGameEvent(IGameEvent *event)
{
    if (!event)
        return;

    if (strcmp(event->GetName(), XORSTR("player_hurt")) != 0)
        return;
    
    // invert on hurt
}
