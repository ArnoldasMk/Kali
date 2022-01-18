#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "antiaim.h"

#include "legitbot.h"
#include "autowall.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"
#include "../Utils/draw.h"
#include "../SDK/vector.h"
#include "fakelag.h"
#define GetPercentVal(val, percent) (val * (percent / 100.f))
#define TICK_INTERVAL (globalVars->interval_per_tick)
#define TICKS_TO_TIME(t) (TICK_INTERVAL * (t))
#ifndef LessThan
#define LessThan(x, y) (x < x)
#endif

#ifndef IsEqual
#define IsEqual(x, y) (x == x)
#endif
bool should_sidemove;
QAngle AntiAim::LastTickViewAngle;
static bool needToFlick = false;
int downpack;
static float NormalizeAsYaw(QAngle &angle)
{

    while (angle.y > 180.f)
        angle.y -= 360.f;

    while (angle.y < -180.f)
        angle.y += 360.f;
}

bool AntiAim::casualcheck()
{
    static ConVar *gaem = cvar->FindVar("game_mode");
    static int gamemode = gaem->GetInt();

    if (!(*csGameRules)->IsValveDS())
        return false;
    if ((*csGameRules)->IsValveDS() && gamemode != 1 && gamemode != 2)
        return true;
    return false;
}

float AntiAim::GetMaxDelta(CCSGOAnimState *animState)
{
    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2)); // - 1.f
    }

    delta = *(float *)((uintptr_t)animState + 0x3A4) * unk2;
    if (AntiAim::casualcheck())
        return 29.0f;
    return delta - 0.5f;
}

void Sidemove(CUserCmd *cmd)
{
    if (!should_sidemove)
        return;
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    float sideAmount = 2 * ((cmd->buttons & IN_DUCK || cmd->buttons & IN_WALK) ? 3.f : 0.505f);
    if (localplayer->GetVelocity().Length2D() <= 0.f || std::fabs(localplayer->GetVelocity().z) <= 100.f)
        cmd->sidemove += cmd->command_number % 2 ? sideAmount : -sideAmount;
}

void ClampAngles(QAngle &angles)
{
    // if (angles.x > 89.0f) angles.x = 89.0f;
    // else if (angles.x < -89.0f) angles.x = -89.0f;

    if (angles.y > 180.0f)
        angles.y = 180.0f;
    else if (angles.y < -180.0f)
        angles.y = -180.0f;

    angles.z = 0;
}
void FixAngles(QAngle &angles)
{
    Math::NormalizeAngles(angles);
    ClampAngles(angles);
}

static C_BasePlayer *GetClosestEnemy(CUserCmd *cmd)
{
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return nullptr;
    C_BasePlayer *closestPlayer = nullptr;
    Vector pVecTarget = localplayer->GetEyePosition();
    QAngle viewAngles;
    engine->GetViewAngles(viewAngles);
    float prevFOV = 0.f;

    int maxPlayers = engine->GetMaxClients();
    for (int i = 1; i < maxPlayers; ++i)
    {
        C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

        if (!player || player == localplayer || player->GetDormant() || !player->GetAlive() || player->GetImmune())
            continue;

        if (Entity::IsTeamMate(player, localplayer))
            continue;

        Vector cbVecTarget = player->GetAbsOrigin();

        float cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));

        if (prevFOV == 0.f)
        {
            prevFOV = cbFov;
            closestPlayer = player;
        }
        else if (cbFov < prevFOV)
        {
            prevFOV = cbFov;
            closestPlayer = player;
        }
    }
    return closestPlayer;
}

static float GetBestHeadAngle(CUserCmd *cmd)
{
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer || !localplayer->GetAlive())
        return 0;

    // C_BasePlayer* target = GetClosestEnemy(cmd);
    // we will require these eye positions
    Vector eye_position = localplayer->GetEyePosition();
    // lets set up some basic values we need
    int best_damage = localplayer->GetHealth();
    // this will result in a 45.0f deg step, modify if you want it to be more 'precise'
    static const float angle_step = 45.f;
    // our result
    float yaw = 0.0f;
    for (int i = 1; i <= engine->GetMaxClients(); i++)
    {
        C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

        if (!player ||
            player == localplayer ||
            player->GetDormant() ||
            !player->GetAlive() ||
            player->GetImmune())
            continue;

        if (Entity::IsTeamMate(player, localplayer))
            return 0;
        // iterate through 45.0f deg angles
        for (float n = 0.0f; n < 180.f; n += angle_step)
        {
            // shoutout to aimtux for headpos calc
            Vector head_position(cos(n) + eye_position.x,
                                 sin(n) + eye_position.y,
                                 eye_position.z);

            AutoWall::FireBulletData data;
            int damage = AutoWall::GetDamage(player, head_position, true, data);
            // cvar->ConsoleDPrintf(XORSTR("AUTO DIRECTION DAMAGE : %d \n"), damage);
            if (damage < best_damage && damage > -1)
            {
                best_damage = damage;
                yaw = n;
            }
        }
    }
    // cvar->ConsoleDPrintf(XORSTR("AUTO DIRECTION yaw : %f \n"), yaw);
    return yaw;
}

void angle_vectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up)
{
    auto sin_cos = [](float radian, float *sin, float *cos)
    {
        *sin = std::sin(radian);
        *cos = std::cos(radian);
    };

    float sp, sy, sr, cp, cy, cr;

    sin_cos(M_PI / 180.0f * angles.x, &sp, &cp);
    sin_cos(M_PI / 180.0f * angles.y, &sy, &cy);
    sin_cos(M_PI / 180.0f * angles.z, &sr, &cr);

    if (forward)
    {
        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    if (right)
    {
        right->x = -1.0f * sr * sp * cy + -1.0f * cr * -sy;
        right->y = -1.0f * sr * sp * sy + -1.0f * cr * cy;
        right->z = -1.0f * sr * cp;
    }

    if (up)
    {
        up->x = cr * sp * cy + -sr * -sy;
        up->y = cr * sp * sy + -sr * cy;
        up->z = cr * cp;
    }
}

static bool GetBestHeadAngle(CUserCmd *cmd, QAngle &angle)
{
    float b, r, l;

    Vector src3D, dst3D, forward, right, up, src, dst;

    trace_t tr;
    Ray_t ray, ray2, ray3, ray4, ray5;
    CTraceFilter filter;

    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer)
        return false;

    QAngle viewAngles;
    engine->GetViewAngles(viewAngles);

    viewAngles.x = 0;

    Math::AngleVectors(viewAngles, &forward, &right, &up);

    auto GetTargetEntity = [&](void)
    {
        int bestDamage = localplayer->GetHealth();
        C_BasePlayer *bestTarget = nullptr;
        for (int i = 0; i < engine->GetMaxClients(); ++i)
        {
            C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

            if (!player || player == localplayer || player->GetDormant() || !player->GetAlive() || player->GetImmune() || player->GetTeam() == localplayer->GetTeam())
                continue;

            // float fov = Math::GetFov(viewAngles, Math::CalcAngle(localplayer->GetEyePosition(), player->GetEyePosition()));

            AutoWall::FireBulletData data;
            int damage = AutoWall::GetDamage(player, localplayer->GetEyePosition(), true, data);

            if (damage < 0)
                continue;
            else if (damage >= bestDamage)
            {
                bestDamage = damage;
                bestTarget = player;
            }
        }

        return bestTarget;
    };

    C_BasePlayer *target = GetTargetEntity();
    filter.pSkip = localplayer;
    src3D = localplayer->GetEyePosition();
    dst3D = src3D + (forward * 384);

    if (target == nullptr)
        return false;

    ray.Init(src3D, dst3D);
    trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
    b = (tr.endpos - tr.startpos).Length();

    ray2.Init(src3D + right * 35, dst3D + right * 35);
    trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
    r = (tr.endpos - tr.startpos).Length();

    ray3.Init(src3D - right * 35, dst3D - right * 35);
    trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
    l = (tr.endpos - tr.startpos).Length();

    if (b < r && b < l && l == r)
        return true; // if left and right are equal and better than back

    if (b > r && b > l)
        AntiAim::realAngle.y = angle.y -= 180; // if back is the best angle
    else if (r > l && r > b)
        AntiAim::realAngle.y = angle.y += 90; // if right is the best angle
    else if (r > l && r == b)
        AntiAim::realAngle.y = angle.y += 135; // if right is equal to back
    else if (l > r && l > b)
        AntiAim::realAngle.y = angle.y -= 90; // if left is the best angle
    else if (l > r && l == b)
        AntiAim::realAngle.y = angle.y -= 135; // if left is equal to back
    else
        return false;

    return true;
}
bool automatic_direction()
{

    float Right, Left;
    Vector src3D, dst3D, forward, right, up;
    trace_t tr;
    Ray_t ray_right, ray_left;
    CTraceFilter filter;

    QAngle engineViewAngles;
    engine->GetViewAngles(engineViewAngles);
    engineViewAngles.x = 0.0f;
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    angle_vectors(engineViewAngles, &forward, &right, &up);

    filter.pSkip = localplayer;
    src3D = localplayer->GetEyePosition();
    dst3D = src3D + forward * 100.0f;

    ray_right.Init(src3D + right * 35.0f, dst3D + right * 35.0f);

    trace->TraceRay(ray_right, MASK_SOLID & ~CONTENTS_MONSTER, &filter, &tr);

    Right = (tr.endpos - tr.startpos).Length();

    ray_left.Init(src3D - right * 35.0f, dst3D - right * 35.0f);

    trace->TraceRay(ray_left, MASK_SOLID & ~CONTENTS_MONSTER, &filter, &tr);

    Left = (tr.endpos - tr.startpos).Length();

    static auto left_ticks = 0;
    static auto right_ticks = 0;

    if (Left - Right > 10.0f)
        left_ticks++;

    else
        left_ticks = 0;

    if (Right - Left > 10.0f)
        right_ticks++;

    else
        right_ticks = 0;

    if (right_ticks > 10)
        return true;

    else if (left_ticks > 10)
        return false;

    return false;
}

bool fake_head()
{
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer)
        return false;

    float server_time = TICKS_TO_TIME(localplayer->GetTickBase());
    static float next_break = 0.f;

    auto animstate = localplayer->GetAnimState();
    if (!animstate)
        return false;

    if (localplayer->GetVelocity().Length2D() > 6.0f || fabs(animstate->verticalVelocity) > 100.f)
        next_break = server_time + 1.1f;

    if (next_break < server_time)
    {
        next_break = server_time + 1.1f;
        return true;
    }

    return false;
}

static bool LBYBreak(float offset, QAngle &angle, C_BasePlayer *localplayer)
{

    static bool lbyBreak;
    lbyBreak = false;
    static float lastCheck = 0.f;
    float vel2D = localplayer->GetVelocity().Length2D();
    if (vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN)
    {
        lbyBreak = false;
        lastCheck = globalVars->curtime;
    }
    else
    {
        if (!lbyBreak && (globalVars->curtime - lastCheck) > 0.22)
        {
            angle.y = offset;
            angle.x = CreateMove::lastTickViewAngles.x;
            // AntiAim::realAngle = angle;
            lbyBreak = true;
            // cvar->ConsoleDPrintf(XORSTR("Lby Break"));
            lastCheck = globalVars->curtime;
            CreateMove::sendPacket = AntiAim::bSend = false;
        }
        else if (lbyBreak && (globalVars->curtime - lastCheck) > 1.1)
        {
            lbyBreak = false;
            lastCheck = globalVars->curtime;
        }
    }

    return lbyBreak;
}

bool AntiAim::LbyUpdate()
{
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    float lastLBY;
    float nextLBY;
    if (localplayer->GetVelocity().x > 0.f || localplayer->GetVelocity().y > 0.f || localplayer->GetVelocity().z > 0.f)
    {
        return false;
        nextLBY = 0.22f;
    }
    if (globalVars->curtime - lastLBY - nextLBY >= 0)
    {
        nextLBY = 1.1f;
        lastLBY = globalVars->curtime;
        return true;
    }
    else
    {
        return false;
    }
}

static void AirAntiAim(C_BasePlayer *const localplayer, CUserCmd *cmd, QAngle &angle)
{
    using namespace Settings::AntiAim::RageAntiAim;

    if (!localplayer || !localplayer->GetAlive())
        return;
    if (localplayer->GetFlags() & FL_ONGROUND)
        return;
    double factor;
    should_sidemove = false;
    factor = 360.0 / M_PHI;
    factor *= JitterPercent;
    angle.y = fmodf(globalVars->curtime * factor, 360.0);
}

bool should_break_lby(CUserCmd *m_pcmd)
{

    // if (FakeLag::ticks > 14)
    //    {
    //              CreateMove::sendPacket = true;
    //        }
    const auto netchannel = GetLocalClient(-1)->m_NetChannel;
    auto packet = netchannel->m_nChokedPackets;
    if (packet > 14)
        CreateMove::sendPacket = true;
    int next_lby_update;

    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer)
        return false;

    auto animstate = localplayer->GetAnimState();

    if (!animstate)
        return false;

    if (localplayer->GetVelocity().x > 0.f || localplayer->GetVelocity().y > 0.f || localplayer->GetVelocity().z > 0.f)
        next_lby_update = TICKS_TO_TIME(localplayer->GetTickBase() + 14);
    else
    {
        if (TICKS_TO_TIME(localplayer->GetTickBase()) > next_lby_update)
        {
            next_lby_update = 0.0f;
            return true;
        }
    }

    return false;
}
static void CustomAA(C_BasePlayer *const localplayer, QAngle &angle, CUserCmd *cmd)
{
    using namespace Settings::AntiAim::RageAntiAim::customaa;
    bool sw = false;
    if (Settings::AntiAim::airspin::enabled && !(localplayer->GetFlags() & FL_ONGROUND))
        return;

    if (!enabled)
        return;

    should_sidemove = sidemove;
    angle.y += baseAngle;
    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    if (!CreateMove::sendPacket)
    {
        if (localplayer->GetVelocity().Length() <= 5.0f && standang != 0)
            angle.y += randang ? standang : (rand() % (standang - (-standang) + 1) + -standang); // stand
        else if (!(localplayer->GetFlags() & FL_ONGROUND) && airang != 0)
            angle.y += randang ? airang : (rand() % (airang - (-airang) + 1) + -airang); // air
        else if (!Settings::SlowWalk::enabled || !inputSystem->IsButtonDown(Settings::SlowWalk::key))
            angle.y += randang ? moveang : (rand() % (moveang - (-moveang) + 1) + -moveang); // move
        else if (Settings::SlowWalk::enabled && inputSystem->IsButtonDown(Settings::SlowWalk::key))
            angle.y += randang ? slowang : (rand() % (slowang - (-slowang) + 1) + -slowang); // slow walk
        else if (Settings::AntiAim::FakeDuck::enabled && inputSystem->IsButtonDown(Settings::AntiAim::FakeDuck::fakeDuckKey))
            angle.y += randang ? duckang : (rand() % (duckang - (-duckang) + 1) + -duckang);
        ; // Fakeduck
    }
    if (AntiAim::LbyUpdate() && lbyang != 0)
        angle.y += lbyang; // lby update
}
static void FreeStand(C_BasePlayer *const localplayer, QAngle &angle, CUserCmd *cmd)
{

    using namespace AntiAim::ManualAntiAim;
    using namespace Settings::AntiAim::RageAntiAim;
    bool sw = false;
    if (Settings::AntiAim::airspin::enabled && !(localplayer->GetFlags() & FL_ONGROUND))
        return;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

    if (legitkey::enabled && cmd->buttons & IN_USE)
    {
        should_sidemove = true;
        if (!CreateMove::sendPacket)
        {
            angle.y += inverted ? maxDelta : maxDelta * -1;
        }

        return;
    }
    if (Settings::AntiAim::ManualAntiAim::Enabled)
    {
        if (Settings::AntiAim::AutoInvert)
        {
            if (automatic_direction())
            {
                alignLeft = true;
                alignRight = false;
            }
            else
            {
                alignRight = true;
                alignLeft = false;
            }
            if (alignLeft)
            {
                angle.y += 70.f;
            }
            else if (alignBack)
            {
                angle.y -= 180.f;
            }
            else if (alignRight)
            {
                angle.y -= 70.f;
            }
        }
        else
        {
            if (alignLeft)
            {
                angle.y += 70.f;
            }
            else if (alignBack)
            {
                angle.y -= 180.f;
            }
            else if (alignRight)
            {
                angle.y -= 70.f;
            }
        }
    }
    else if (!Settings::AntiAim::HeadEdge::enabled)
    {
        if (lby::enabled)
        {
            angle.y -= 180.f;

            if (should_break_lby(cmd) && !(AntiAim::casualcheck()))
            {
                if (CreateMove::sendPacket)
                {
                    auto speed = 1.01f;
                    static auto sway_counter = 0;

                    if (cmd->buttons & IN_DUCK)
                        speed *= 2.94117647f;

                    static auto switch_move = false;

                    if (switch_move)
                        cmd->sidemove += speed;
                    else
                        cmd->sidemove -= speed;

                    switch_move = !switch_move;

                    if (lby::type == LbyMode::Opposite || sway_counter > 3)
                    {
                        if (inverted)
                            angle.y += 116.0f;
                        else
                            angle.y -= 116.0f;
                    }
                    else if (lby::type == LbyMode::Normal)
                    {
                        if (inverted)
                            angle.y += 22.0f;
                        else
                            angle.y -= 22.0f;
                    }

                    if (sway_counter < 8)
                        ++sway_counter;
                    else
                        sway_counter = 0;
                    //	CreateMove::sendPacket = false;
                }
            }
        }
        else
        {
            angle.y -= 180.f;
        }
    }
    if (Settings::AntiAim::HeadEdge::enabled)
    {
        //    if (GetBestHeadAngle(cmd, angle))
        float headAngle = GetBestHeadAngle(cmd);
        if (headAngle > 0)
            angle.y -= GetBestHeadAngle(cmd);
    }
    double factor;
    bool side;

    static bool buttonToggle = false;
    /* Button Function for invert the fake*/
    if (inputSystem->IsButtonDown(InvertKey) && !buttonToggle)
    {
        buttonToggle = true;
        inverted = !inverted;
    }
    else if (!inputSystem->IsButtonDown(InvertKey) && buttonToggle)
        buttonToggle = false;
    if (fakepeek)
    {
        static bool battonToggle = false;
        if (inputSystem->IsButtonDown(fakeheadkey) && !battonToggle)
        {
            battonToggle = true;
            head = !head;
        }
        else if (!inputSystem->IsButtonDown(fakeheadkey) && battonToggle)
            battonToggle = false;

        if (fake_head() && localplayer->GetVelocity().Length2D() < 6.0f && head)
        {
            if (inverted)
                angle.y += 90.f;
            else
                angle.y -= 90.f;
            CreateMove::sendPacket = true;
        }
    }
    const auto netchannel = GetLocalClient(-1)->m_NetChannel;
    int pakets = netchannel->m_nChokedPackets;

    switch (Settings::AntiAim::Yaw::typeReal)
    {
    case AntiAimRealType_Y::NONE:
        should_sidemove = false;
        break;

    case AntiAimRealType_Y::Static:
        if (!(should_break_lby(cmd) && lby::enabled && !Settings::AntiAim::ManualAntiAim::Enabled))
        {
            should_sidemove = true;
            if (!AntiAim::bSend)
                angle.y += inverted ? (AntiAim::GetMaxDelta(localplayer->GetAnimState()) + AntiAimOffset) : -(AntiAim::GetMaxDelta(localplayer->GetAnimState()) + AntiAimOffset);
        }
        break;

    case AntiAimRealType_Y::Jitter:
        if (!(should_break_lby(cmd) && lby::enabled && !Settings::AntiAim::ManualAntiAim::Enabled))
        {
            should_sidemove = true;
            if (!CreateMove::sendPacket)
                angle.y = inverted ? AntiAim::realAngle.y - (AntiAim::GetMaxDelta(localplayer->GetAnimState()) + 30) : -(AntiAim::realAngle.y - (AntiAim::GetMaxDelta(localplayer->GetAnimState()) + 30));
            if (AntiAim::LbyUpdate())
                angle.y = inverted ? AntiAim::realAngle.y - (AntiAim::GetMaxDelta(localplayer->GetAnimState()) + 30) : -(AntiAim::realAngle.y - (AntiAim::GetMaxDelta(localplayer->GetAnimState()) + 30));
        }
        break;

    case AntiAimRealType_Y::Randome:
        if (!(should_break_lby(cmd) && lby::enabled && !Settings::AntiAim::ManualAntiAim::Enabled))
        {
            should_sidemove = true;
            if (!AntiAim::bSend)
            {
                static auto st = 0;
                st++;
                if (st < 2)
                {
                    static auto j = false;
                    angle.y += j ? JitterPercent : -JitterPercent;
                    // AntiAim::realAngle.y = AntiAim::fakeAngle.y - j ? JitterPercent : -JitterPercent;
                    j = !j;
                }
                else
                    st = 0;
            }
        }
        break;
    case AntiAimRealType_Y::JitterSwitch:
        if (!(should_break_lby(cmd) && lby::enabled && !Settings::AntiAim::ManualAntiAim::Enabled))
        {
            should_sidemove = true;
            if (!(cmd->tick_count % 16))
                sw = !sw;
            angle.y += sw ? JitterPercent : -JitterPercent;
        }
        break;
    case AntiAimRealType_Y::Spin:
        if (!(should_break_lby(cmd) && lby::enabled && !Settings::AntiAim::ManualAntiAim::Enabled))
        {
            should_sidemove = true;
            factor = 360.0 / M_PHI;
            factor *= JitterPercent;
            angle.y = fmodf(globalVars->curtime * factor, 360.0);
        }
        break;

    case AntiAimRealType_Y::JitterRandom:
        if (!(should_break_lby(cmd) && lby::enabled && !Settings::AntiAim::ManualAntiAim::Enabled))
        {
            should_sidemove = true;
            int stuff = JitterPercent;
            float randNum = (rand() % (stuff - (-stuff) + 1) + -stuff);
            if (!AntiAim::bSend)
            {
                //		if (pakets > 6)
                angle.y += randNum;
                //		else
                //           angle.y -= randNum;
                if (angle.y > maxDelta)
                    angle.y = maxDelta;
            }
        }
        break;
    }
    if (AntiAim::casualcheck() && angle.y > maxDelta)
        angle.y = maxDelta;
}

static void DoAntiAimX(QAngle &angle, CUserCmd *cmd)
{
    static auto invert_jitter = false;
    static auto should_invert = false;
    if (Settings::AntiAim::LegitAntiAim::enabled)
        return;
    static auto std = 0;
    std++;
    if (std < 2)
    {
        invert_jitter = !invert_jitter;
    }
    else
        std = 0;
    if (Settings::AntiAim::RageAntiAim::legitkey::enabled && cmd->buttons & IN_USE)
        return;

    switch (Settings::AntiAim::pitchtype)
    {

    case AntiAimType_X::STATIC_UP:
        angle.x = cmd->viewangles.x = -89.0f;
        break;

    case AntiAimType_X::STATIC_DOWN:
        angle.x = cmd->viewangles.x = 89.0f;
        break;

    case AntiAimType_X::FRONT:
        angle.x = cmd->viewangles.x = 0.0f;
        break;

    case AntiAimType_X::EMOTION:
        angle.x = cmd->viewangles.x = 89.9999999999999999999999999999999;
        break;

    case AntiAimType_X::FRONT_FAKE:
        angle.x = cmd->viewangles.x = ((*csGameRules)->IsValveDS()) ? 0.0f : -1080.0f;
        break;

    case AntiAimType_X::DOWN_FAKE:
        angle.x = cmd->viewangles.x = ((*csGameRules)->IsValveDS()) ? -89.0f : -540.0f;
        break;

    case AntiAimType_X::UP_FAKE:
        angle.x = cmd->viewangles.x = ((*csGameRules)->IsValveDS()) ? 89.0f : 540.0f;
        break;

    case AntiAimType_X::FAKE_JITTER:
        angle.x = cmd->viewangles.x = invert_jitter ? (((*csGameRules)->IsValveDS()) ? 89.0f : 540.0f) : (((*csGameRules)->IsValveDS()) ? -89.0f : -540.0f);
        break;
    }
}
static void DoLegitAntiAim(C_BasePlayer *const localplayer, QAngle &angle, bool &bSend, CUserCmd *cmd)
{
    if (!localplayer->GetAlive() || !localplayer)
        return;

    using namespace Settings::AntiAim::LegitAntiAim;

    static bool buttonToggle = false;
    if (inputSystem->IsButtonDown(InvertKey) && !buttonToggle)
    {
        buttonToggle = true;
        inverted = !inverted;
    }
    else if (!inputSystem->IsButtonDown(InvertKey) && buttonToggle)
        buttonToggle = false;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    if (Settings::AntiAim::AutoInvert)
        inverted = !automatic_direction();
    if (Settings::AntiAim::arms)
    {
        static ConVar *hand = cvar->FindVar("cl_righthand");
        hand->SetValue(inverted ? 0 : 1);
    }
    static auto LBYBREAK([&](const float &offset)
                         {
        static bool lbyBreak = false;
        static float lastCheck = 0.f;
        float vel2D = localplayer->GetVelocity().Length2D();
        if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
                lbyBreak = false;
                lastCheck = globalVars->curtime;
            } 
            else {
                if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
                    angle.y = offset;
                    CreateMove::sendPacket =  AntiAim::bSend = false;
                    lbyBreak = true;
                    lastCheck = globalVars->curtime;
                    needToFlick = true;
                } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                    angle.y = offset;
                    CreateMove::sendPacket =  AntiAim::bSend = false;
                    lbyBreak = true;
                    lastCheck = globalVars->curtime;
                    needToFlick = true;
                }
            } });

    QAngle ViewAngle;
    engine->GetViewAngles(ViewAngle);

    static auto OverWatchProof([&]()
                               {
               should_sidemove = true;

if (!CreateMove::sendPacket)
        {
                   angle.y += inverted ? maxDelta : maxDelta*-1;
	 }
        else{
               if (!Settings::FakeLag::enabled)
 		  CreateMove::sendPacket = false;

        }

        inverted ? LBYBREAK(angle.y+maxDelta-1) : LBYBREAK(angle.y-maxDelta-1); });
    static auto FakeLegitAA([&]()
                            {
        if (!AntiAim::bSend)
        {
            localplayer->GetAnimState()->goalFeetYaw = inverted ? angle.y + maxDelta : angle.y - maxDelta;
            AntiAim::realAngle = angle;
        }
        else
            AntiAim::fakeAngle = angle; });
    static auto Experimental([&]()
                             {
	if (AntiAim::casualcheck())
		return;
                auto speed = 1.01f;

                if (cmd->buttons & IN_DUCK)
                        speed *= 2.94117647f;

                static auto switch_move = false;

                if (switch_move)
                        cmd->sidemove += speed;
                else
                        cmd->sidemove -= speed;

                switch_move = !switch_move;
	int chokedticks = 0;
	if (should_break_lby && !CreateMove::sendPacket)
	angle.y -= inverted ? maxDelta * 2 : maxDelta * -2;
               if (!Settings::FakeLag::enabled){
                if (chokedticks < 1){
                        CreateMove::sendPacket = false;
                        chokedticks++;
                }else{
                        CreateMove::sendPacket = true;
			chokedticks = 0;
                }
	} });

    switch (Settings::AntiAim::LegitAntiAim::legitAAtype)
    {
    case LegitAAType::OverWatchProof:
        OverWatchProof();
        break;
    case LegitAAType::FakeLegitAA:
        FakeLegitAA();
        break;
    case LegitAAType::Experimental:
        Experimental();
        break;
    default:
        break;
    }
}

static void DoManuaAntiAim(C_BasePlayer *localplayer, QAngle &angle)
{
    using namespace Settings::AntiAim::RageAntiAim;

    static bool Bpressed = false;

    if (!Settings::AntiAim::ManualAntiAim::Enabled)
        return;

    if (inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !Bpressed)
    {
        AntiAim::ManualAntiAim::alignBack = !AntiAim::ManualAntiAim::alignBack;
        AntiAim::ManualAntiAim::alignLeft = AntiAim::ManualAntiAim::alignRight = false;
        Bpressed = true;
    }
    else if (inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !Bpressed)
    {
        AntiAim::ManualAntiAim::alignRight = !AntiAim::ManualAntiAim::alignRight;
        AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignLeft = false;
        Bpressed = true;
    }
    else if (inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !Bpressed)
    {
        AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignRight = false;
        AntiAim::ManualAntiAim::alignLeft = !AntiAim::ManualAntiAim::alignLeft;
        Bpressed = true;
    }
    bool buttonNotPressed = !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton);

    if (buttonNotPressed && Bpressed)
        Bpressed = false;
}

static bool canMove(C_BasePlayer *localplayer, C_BaseCombatWeapon *activeweapon, CUserCmd *cmd)
{
    if (!activeweapon)
        return false;
    ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
    if (Settings::Legitbot::weapons.find(*activeweapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
        index = *activeweapon->GetItemDefinitionIndex();
    const LegitWeapon_t &currentWeaponSetting = Settings::Legitbot::weapons.at(index);

    if (currentWeaponSetting.aimStepEnabled && Legitbot::aimStepInProgress)
        return false;

    if (activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade *csGrenade = (C_BaseCSGrenade *)activeweapon;
        if (csGrenade->GetThrowTime() > 0.f)
            return false;
    }
    if (*activeweapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
    {
        const float &postponTime = activeweapon->GetPostPoneReadyTime();
        if (cmd->buttons & IN_ATTACK2)
            return false;
        if (postponTime < globalVars->curtime)
            return true;
    }

    if ((*csGameRules)->IsFreezeTime())
        return false;
    if (localplayer->GetFlags() & FL_FROZEN)
        return false;
    if (cmd->buttons & IN_ATTACK)
        return false;
    if (!(Settings::AntiAim::RageAntiAim::legitkey::enabled) && cmd->buttons & IN_USE)
        return false;
    if (cmd->buttons & IN_ATTACK2 && activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return false;
    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return false;

    return true;
}

void AntiAim::CreateMove(CUserCmd *cmd)
{
    if (!Settings::AntiAim::RageAntiAim::enabled && !Settings::AntiAim::LBYBreaker::enabled && !Settings::AntiAim::LegitAntiAim::enabled && !Settings::AntiAim::RageAntiAim::customaa::enabled)
        return;
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    C_BaseCombatWeapon *activeWeapon = (C_BaseCombatWeapon *)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

    if (!activeWeapon)
        return;

    if (!canMove(localplayer, activeWeapon, cmd))
    {
        CreateMove::lastTickViewAngles = AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
        return;
    }

    if (Settings::FakeLag::enabled)
        CreateMove::sendPacket ? AntiAim::bSend = CreateMove::sendPacket : AntiAim::bSend = cmd->command_number % 2;
    else
        AntiAim::bSend = cmd->command_number % 2;
    Sidemove(cmd);
    QAngle angle = cmd->viewangles;
    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    if (Settings::AntiAim::LBYBreaker::enabled)
    {
        LBYBreak(Settings::AntiAim::LBYBreaker::offset, angle, localplayer);
    }
    else if (Settings::AntiAim::RageAntiAim::enabled) // responsible for reage anti aim or varity of anti aims ..
    {
        DoManuaAntiAim(localplayer, angle);
        if (Settings::AntiAim::RageAntiAim::atTheTarget)
        {
            C_BasePlayer *lockedTarget = GetClosestEnemy(cmd);
            if (lockedTarget && Settings::AntiAim::RageAntiAim::legitkey::enabled && !(cmd->buttons & IN_USE))
                angle = Math::CalcAngle(localplayer->GetEyePosition(), lockedTarget->GetEyePosition());
        }
        if (!Settings::AntiAim::RageAntiAim::customaa::enabled)
            FreeStand(localplayer, angle, cmd);
        else
            CustomAA(localplayer, angle, cmd);

        if (Settings::AntiAim::airspin::enabled)
        {
            AirAntiAim(localplayer, cmd, angle);
        }
    }
    else if (Settings::AntiAim::LegitAntiAim::enabled) // Responsible for legit anti aim activated when the legit anti aim is enabled
        DoLegitAntiAim(localplayer, angle, AntiAim::bSend, cmd);
    //          LBYBreak(CreateMove::lastTickViewAngles.y, angle, localplayer);
    Math::NormalizeAngles(angle);
    DoAntiAimX(angle, cmd);

    // angle = NormalizeAsYaw(angle);
    Math::ClampAngles(angle);
    if (!Settings::AntiAim::LegitAntiAim::enabled)
    {
        if (!AntiAim::bSend)
        {
            AntiAim::fakeAngle = angle;
        }
        else
        {
            AntiAim::realAngle = angle;
        }
    }
    else
    {
        if (AntiAim::bSend)
        {
            AntiAim::realAngle = angle;
        }
        else
        {
            AntiAim::fakeAngle = angle;
        }
    }
    if (CreateMove::sendPacket)
        AntiAim::realDuck = localplayer->GetAnimState()->duckProgress;
    CreateMove::lastTickViewAngles = AntiAim::realAngle;

    cmd->viewangles = angle;

    if (!Settings::FakeLag::enabled)
        CreateMove::sendPacket = AntiAim::bSend;
    // Math::ClampAngles(angle);
    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}

void AntiAim::FrameStageNotify(ClientFrameStage_t stage)
{
    if (!Settings::AntiAim::LegitAntiAim::enabled)
        return;

    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

    if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
    {
        if (Settings::AntiAim::LegitAntiAim::legitAAtype == LegitAAType::Experimental)
        {
            using namespace Settings::AntiAim::LegitAntiAim;
            static bool buttonToggle = false;

            float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

            QAngle angle;
            engine->GetViewAngles(angle);

            AntiAim::realAngle.y = localplayer->GetAnimState()->goalFeetYaw = inverted ? angle.y + 27.f : angle.y - 27.f;
        }
    }
}

void AntiAim::OverrideView(CViewSetup *pSetup)
{
    if (!Settings::AntiAim::RageAntiAim::enabled && !Settings::AntiAim::LBYBreaker::enabled && !Settings::AntiAim::LegitAntiAim::enabled)
        return;

    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer || !localplayer->GetAlive())
        return;

    // pSetup->origin.x = localplayer->GetAbsOrigin().x + 64.0f;
}
void AntiAim::FireGameEvent(IGameEvent *event)
{
    if (!event)
        return;

    if (strcmp(event->GetName(), XORSTR("player_hurt")) != 0)
        return;
    int hurt_player_id = event->GetInt(XORSTR("userid"));
    if (engine->GetPlayerForUserID(hurt_player_id) != engine->GetLocalPlayer())
        return;
    if (!Settings::AntiAim::RageAntiAim::invertOnHurt)
        return;

    Settings::AntiAim::RageAntiAim::inverted = !Settings::AntiAim::RageAntiAim::inverted;
}
