#include "Tickbase.h"
#include <climits>
#include "../SDK/INetChannel.h"
#include "ragebot.h"
#include "../Utils/draw.h"
#include "fakelag.h"
#include "../Utils/patternfinder.h"
#include "../SDK/definitions.h"

bool canShift(int ticks, bool shiftAnyways = false)
{
        C_BasePlayer* localPlayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
        C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localPlayer->GetActiveWeapon());

    if (!localPlayer || !localPlayer->GetAlive()|| !Settings::Ragebot::exploits::enabled || ticks <= 0)
        return false;

    if (shiftAnyways)
        return true;

    if ((Tickbase::tick->ticksAllowedForProcessing - ticks) < 0)
        return false;

    if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
        return false;

    float nextAttack = (activeWeapon->GetNextPrimaryAttack() + (ticks * globalVars->interval_per_tick));
    if (nextAttack >=  globalVars->curtime)
        return false;

    if (!activeWeapon)// || activeWeapon->throwing()) commented this bc it needs UserCmd* and im too lazy 2 fix
        return false;
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();

    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
        return false;

    float shiftTime = (localPlayer->GetTickBase() - ticks) * globalVars->interval_per_tick;

    if (shiftTime < activeWeapon->GetNextPrimaryAttack())
        return false;

    return true;
}
//THANK YOU DoomFishWasTaken and everyone who worked on this https://github.com/danielkrupinski/Osiris/pull/2074
//static bool  WriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, void* buffer, int from, int to, bool isnewcommand) noexcept
//{
/*
    if (from != -1)
        return true;

    int* numBackupCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x30);
    int* numNewCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x2C);

    int32_t newcommands = *numNewCommands;

    int nextcommmand;
    int totalcommands = std::min(Tickbase::tick->tickshift, Tickbase::tick->maxUsercmdProcessticks);
    Tickbase::tick->tickshift = 0;

    from = -1;
    *numNewCommands = totalcommands;
    *numBackupCommands = 0;

    for (to = nextcommmand - newcommands + 1; to <= nextcommmand; to++)
    {
        from = to;
    }

    //UserCmd* lastRealCmd = memory->input->GetUserCmd(slot, from);
    UserCmd* fromcmd;

   // if (lastRealCmd)
     //   fromcmd = *lastRealCmd;

    UserCmd tocmd = fromcmd;
    tocmd.tickCount += 200;
    tocmd.commandNumber++;

    for (int i = newcommands; i <= totalcommands; i++)
    {
        //WriteUsercmd(buffer, &tocmd, &fromcmd);
        fromcmd = tocmd;
        tocmd.commandNumber++;
        tocmd.tickCount++;
    }

    return true;
*/
//}
void recalculateTicks() 
{
    Tickbase::tick->chokedPackets = std::clamp(Tickbase::tick->chokedPackets, 0, Tickbase::tick->maxUsercmdProcessticks);
    Tickbase::tick->ticksAllowedForProcessing = Tickbase::tick->maxUsercmdProcessticks - Tickbase::tick->chokedPackets;
    Tickbase::tick->ticksAllowedForProcessing = std::clamp(Tickbase::tick->ticksAllowedForProcessing, 0, Tickbase::tick->maxUsercmdProcessticks);
}

void Tickbase::shiftTicks(int ticks, CUserCmd* cmd, bool shiftAnyways) noexcept //useful, for other funcs
{

C_BasePlayer* localPlayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localPlayer || !localPlayer->GetAlive()|| !Settings::Ragebot::exploits::enabled)
        return;
    if (!canShift(ticks, shiftAnyways))
        return;

    tick->commandNumber = cmd->command_number;
    tick->tickbase = localPlayer->GetTickBase();
    tick->tickshift = ticks;
    Tickbase::lastShift = cmd->command_number;
    //Teleport kinda buggy
    //tick->chokedPackets += ticks;
    //recalculateTicks();
}
//auto Tickbase::WriteUsercmd = [](bf_write* buf, CUserCmd* in, CUserCmd* out) {
//        using WriteUsercmd_t = void(bf_write*, CUserCmd*, CUserCmd*);
//        WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)PatternFinder::FindPatternInModule("client_client.so", (unsigned char*) XORSTR( "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x8B\xD9\x8B\x0D"), XORSTR("xxxxxxxxxxxxxxxxxx"));
//        WriteUsercmdF(buf, in, out);

//    };

//auto stuff(void* pThis, const char* name){
// static auto find_hud_element = reinterpret_cast<(void*, const char*)>(PatternFinder::FindPatternInModule(XORSTR("/client_client.so"),(unsigned char *)XORSTR("\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x57\x8B\xF9\x33\xF6\x39\x77\x28"),XORSTR("xxxxxxxxxxxxxxxxxxx"))));
//    return find_hud_element(pThis, name);
//}
void Tickbase::run(CUserCmd* cmd, bool& sendPacket) noexcept
{
        C_BasePlayer* localPlayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localPlayer || !localPlayer->GetAlive()|| !Settings::Ragebot::exploits::enabled)
        return;


    if (Settings::Ragebot::exploits::doubletapKey) {
            if (!inputSystem->IsButtonDown(Settings::Ragebot::exploits::doubletapKey))
            {
                 Settings::Ragebot::exploits::doubletapToggle = false;
            }
            else
                 Settings::Ragebot::exploits::doubletapToggle = true;
    }
    else
         Settings::Ragebot::exploits::doubletapToggle = true;

    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / globalVars->interval_per_tick); };
const auto netchannel = GetLocalClient(-1)->m_NetChannel;

    static void* oldNetwork = nullptr;
   if (auto network = netchannel; network && oldNetwork != network)
    {
        oldNetwork = network;
        tick->ticksAllowedForProcessing = tick->maxUsercmdProcessticks;
        tick->chokedPackets = 0;
    } // Commenting out until i decide to do it.
    if (FakeLag::ticks > tick->chokedPackets)
        tick->chokedPackets = FakeLag::ticks;

    recalculateTicks();

    tick->ticks = cmd->tick_count;
    if (!localPlayer || !localPlayer->GetAlive()|| !Settings::Ragebot::exploits::enabled)
        return;

    auto ticks = 0;

    switch (Settings::Ragebot::exploits::doubletapSpeed) {
    case DtSpeed::INSTANT: //Instant
        ticks = 16;
        break;
    case DtSpeed::FAST: //Fast
        ticks = 14;
        break;
    case DtSpeed::ACCURATE: //Accurate
        ticks = 12;
        break;
    }
//You can shift 9 ticks to do hideshots without doubletapping
    if (Settings::Ragebot::exploits::doubletap && cmd->buttons & (IN_ATTACK) && Settings::Ragebot::exploits::doubletapToggle)
        shiftTicks(ticks, cmd);

    if (tick->tickshift <= 0 && tick->ticksAllowedForProcessing < (tick->maxUsercmdProcessticks - tick->fakeLag) && !inputSystem->IsButtonDown(Settings::AntiAim::FakeDuck::fakeDuckKey) && (cmd->command_number - lastShift) >= tick->maxUsercmdProcessticks && Settings::Ragebot::exploits::doubletapToggle) //&& ((config->misc.fakeLagMode != 0 && config->misc.fakeLagTicks <= (tick->maxUsercmdProcessticks - ticks)) || !config->misc.fakeLagMode))
    {
        CreateMove::sendPacket = true;
        cmd->tick_count = INT_MAX; //recharge
        tick->chokedPackets--;
    }

    recalculateTicks();
}
