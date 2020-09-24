
// //Tickbase.cpp //must be created on hacks same for Tickbase.h
// //#include "Backtrack.h"
// //#include "Tickbase.h"

// #include "../SDK/Entity.h"
// #include "../SDK/UserCmd.h"

// bool canShift(int ticks, bool shiftAnyways = false)
// {
//     if (!localPlayer || !localPlayer->isAlive() || !config->ragebotExtra.enabled || ticks <= 0)
//         return false;

//     if (shiftAnyways)
//         return true;

//     if ((Tickbase::tick->ticksAllowedForProcessing - ticks) < 0)
//         return false;

//     if (localPlayer->nextAttack() > memory->globalVars->serverTime())
//         return false;

//     float nextAttack = (localPlayer->nextAttack() + (ticks * memory->globalVars->intervalPerTick));
//     if (nextAttack >= memory->globalVars->serverTime())
//         return false;

//     auto activeWeapon = localPlayer->getActiveWeapon();
//     if (!activeWeapon || !activeWeapon->clip() || activeWeapon->isThrowing())
//         return false;

//     if (activeWeapon->isKnife() || activeWeapon->isGrenade() || activeWeapon->isShotgun()
//         || activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver
//         || activeWeapon->itemDefinitionIndex2() == WeaponId::Awp
//         || activeWeapon->itemDefinitionIndex2() == WeaponId::Ssg08
//         || activeWeapon->itemDefinitionIndex2() == WeaponId::Taser
//         || activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver)
//         return false;

//     float shiftTime = (localPlayer->tickBase() - ticks) * memory->globalVars->intervalPerTick;

//     if (shiftTime < activeWeapon->nextPrimaryAttack())
//         return false;

//     return true;
// }

// void recalculateTicks() noexcept
// {
//     Tickbase::tick->chokedPackets = std::clamp(Tickbase::tick->chokedPackets, 0, Tickbase::tick->maxUsercmdProcessticks);
//     Tickbase::tick->ticksAllowedForProcessing = Tickbase::tick->maxUsercmdProcessticks - Tickbase::tick->chokedPackets;
//     Tickbase::tick->ticksAllowedForProcessing = std::clamp(Tickbase::tick->ticksAllowedForProcessing, 0, Tickbase::tick->maxUsercmdProcessticks);
// }

// void Tickbase::shiftTicks(int ticks, UserCmd* cmd, bool shiftAnyways) noexcept //useful, for other funcs
// {
//     if (!localPlayer || !localPlayer->isAlive() || !config->ragebotExtra.enabled)
//         return;
//     if (!canShift(ticks, shiftAnyways))
//         return;
//     tick->commandNumber = cmd->commandNumber;
//     tick->tickbase = localPlayer->tickBase();
//     tick->tickshift = ticks;
//     //Teleport kinda buggy
//     //tick->chokedPackets += ticks;
//     //recalculateTicks();
// }

// void Tickbase::run(UserCmd* cmd) noexcept
// {

//     static void* oldNetwork = nullptr;
//     if(auto network = interfaces->engine->getNetworkChannel(); network && oldNetwork != network)
//     {
//         oldNetwork = network;
//         tick->ticksAllowedForProcessing = tick->maxUsercmdProcessticks;
//         tick->chokedPackets = 0;
//     }
//     if (auto network = interfaces->engine->getNetworkChannel(); network && network->chokedPackets > tick->chokedPackets)
//         tick->chokedPackets = network->chokedPackets;

//     recalculateTicks();

//     tick->ticks = cmd->tickCount;
//     if (!localPlayer || !localPlayer->isAlive() || !config->ragebotExtra.enabled)
//         return;

//     auto ticks = 0;

//     switch (config->ragebotExtra.doubletapSpeed) {
//     case 0: //Instant
//         ticks = 16;
//         break;
//     case 1: //Fast
//         ticks = 14;
//         break;
//     case 2: //Accurate
//         ticks = 12;
//         break;
//     }

//     if (config->ragebotExtra.doubletap && cmd->buttons & (UserCmd::IN_ATTACK))
//         shiftTicks(ticks, cmd);

//     if (tick->tickshift <= 0 && tick->ticksAllowedForProcessing < (tick->maxUsercmdProcessticks - tick->fakeLag) && !config->antiAim.fakeDucking && ((config->antiAim.fakeLag && config->antiAim.fakeLagAmount <= (tick->maxUsercmdProcessticks - ticks)) || !config->antiAim.fakeLag))
//     {
//         cmd->tickCount = INT_MAX; //recharge
//         tick->chokedPackets--;
//     }

//     recalculateTicks();
// }
// Tickbase.h
// #pragma once

// struct UserCmd;

// namespace Tickbase
// {
// 	void shiftTicks(int, UserCmd*, bool = false) noexcept;
// 	void run(UserCmd*) noexcept;

// 	struct Tick
// 	{
// 		int	maxUsercmdProcessticks{ 17 }; //on valve servers this is 8 ticks, always do +1 command
// 		int ticksAllowedForProcessing{ maxUsercmdProcessticks };
// 		int chokedPackets{ 0 };
// 		int fakeLag{ 0 };
// 		int tickshift{ 0 };
// 		int tickbase{ 0 };
// 		int commandNumber{ 0 };
// 		int ticks{ 0 };
// 	};
// 	inline std::unique_ptr<Tick> tick;
// }
// Memory.cpp
// WriteUsercmdDeltaToBufferReturn = *(reinterpret_cast<void**>(findPattern(L"engine", "\x84\xC0\x74\x04\xB0\x01\xEB\x02\x32\xC0\x8B\xFE\x46\x3B\xF3\x7E\xC9\x84\xC0\x0F\x84????")));
// WriteUsercmd = findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x8B\xD9\x8B\x0D");
// Memory.h
// void* WriteUsercmdDeltaToBufferReturn;
// uintptr_t WriteUsercmd;
// Hooks.cpp
// //createmove
// Tickbase::run(cmd);//run this after ragebot
// Tickbase::tick = std::make_unique<Tickbase::Tick>();//put this on once on "wndProc"
// void WriteUsercmd(void* buf, UserCmd* in, UserCmd* out) 
// {
//     static DWORD WriteUsercmdF = (DWORD)memory->WriteUsercmd;

//     __asm
//     {
//         mov ecx, buf
//         mov edx, in
//         push out
//         call WriteUsercmdF
//         add esp, 4
//     }
// }

// static bool __fastcall WriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, void* buffer, int from, int to, bool isnewcommand) noexcept
// {
//     auto original = hooks->client.getOriginal<bool, int, void*, int, int, bool>(24, slot, buffer, from, to, isnewcommand);

//     if(_ReturnAddress() == memory->WriteUsercmdDeltaToBufferReturn || Tickbase::tick->tickshift <= 0 || !memory->clientState)
//         return original(ecx, slot, buffer, from, to, isnewcommand);

//     if (from != -1)
//         return true;

//     int* numBackupCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x30);
//     int* numNewCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x2C);

//     int32_t newcommands = *numNewCommands;

//     int nextcommmand = memory->clientState->lastOutgoingCommand + memory->clientState->chokedCommands + 1;
//     int totalcommands = std::min(Tickbase::tick->tickshift, Tickbase::tick->maxUsercmdProcessticks);
//     Tickbase::tick->tickshift = 0;

//     from = -1;
//     *numNewCommands = totalcommands;
//     *numBackupCommands = 0;

//     for (to = nextcommmand - newcommands + 1; to <= nextcommmand; to++)
//     {
//         if (!(original(ecx, slot, buffer, from, to, true)))
//             return false;

//         from = to;
//     }

//     UserCmd* lastRealCmd = memory->input->GetUserCmd(slot, from);
//     UserCmd fromcmd;

//     if (lastRealCmd)
//         fromcmd = *lastRealCmd;

//     UserCmd tocmd = fromcmd;
//     tocmd.tickCount += 200;
//     tocmd.commandNumber++;

//     for (int i = newcommands; i <= totalcommands; i++)
//     {
//         WriteUsercmd(buffer, &tocmd, &fromcmd);
//         fromcmd = tocmd;
//         tocmd.commandNumber++;
//         tocmd.tickCount++;
//     }

//     return true;
// }
// //hookat
// client.hookAt(24, WriteUsercmdDeltaToBuffer);
