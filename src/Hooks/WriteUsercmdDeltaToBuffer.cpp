// #include "hooks.h"
// #include <future>
// #include "../interfaces.h"
// #include "../settings.h"


// #include "../Utils/patternfinder.h"

//     bool Hooks::hkdWriteUsercmdDeltaToBuffer(void* ecx, void*, int slot, bf_write* buf, int from, int to, bool isnewcommand)
//     {
//  auto WriteUsercmdDeltaToBufferFn = ( IBaseClientDLL, void*, int, bf_write*, int, int, bool );
//         static auto ofunct = clientModeVMT->GetOriginalMethod<WriteUsercmdDeltaToBufferFn>(24);

//         if (m_nTickbaseShift <= 0)
//                 return ofunct(ecx, slot, buf, from, to, isnewcommand);

//         if (from != -1)
//                 return true;

//         auto CL_SendMove = []() {
//                 using CL_SendMove_t = void(__fastcall*)(void);
//                 static CL_SendMove_t CL_SendMoveF = (CL_SendMove_t)PatternFinder::FindPatternInModule(XORSTR("/engine.so"),XORSTR("55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98"));

//                 CL_SendMoveF();
//         };
//         auto WriteUsercmd = [](bf_write* buf, CUserCmd* in, CUserCmd* out) {
//                 using WriteUsercmd_t = void(__fastcall*)(bf_write*, CUserCmd*, CUserCmd*);
//                 static WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)PatternFinder::FindPatternInModule(XORSTR("/client_client.so"),XORSTR("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));
//                 WriteUsercmdF(buf, in, out);

//         };

//         int* pNumBackupCommands = (int*)(reinterpret_cast <uintptr_t> (buf) - 0x30);
//         int* pNumNewCommands = (int*)(reinterpret_cast <uintptr_t> (buf) - 0x2C);
//         auto net_channel = *reinterpret_cast <NetChannel * *> (reinterpret_cast <uintptr_t> (g_pClientState) + 0x9C);
//         int32_t new_commands = *pNumNewCommands;

//         int32_t next_cmdnr = g_pClientState->m_nLastOutgoingCommand + g_pClientState->m_nChokedCommands + 1;
//         int32_t total_new_commands = min(Globals::m_nTickbaseShift, 16);
//         Globals::m_nTickbaseShift -= total_new_commands;

//         from = -1;
//         *pNumNewCommands = total_new_commands;
//         *pNumBackupCommands = 0;

//         for (to = next_cmdnr - new_commands + 1; to <= next_cmdnr; to++) {
//                 if (!ofunct(ecx, slot, buf, from, to, isnewcommand))
//                         return false;

//                 from = to;
//         }

//         CUserCmd* last_realCmd = g_GameInput->GetUserCmd(slot, from);
//         CUserCmd fromCmd;

//         if (last_realCmd)
//                 fromCmd = *last_realCmd;

//         CUserCmd toCmd = fromCmd;
//         toCmd.command_number++;
//         toCmd.tick_count += 200;

//         for (int i = new_commands; i <= total_new_commands; i++) {
//                 WriteUsercmd(buf, &toCmd, &fromCmd);
//                 fromCmd = toCmd;
//                 toCmd.command_number++;
//                 toCmd.tick_count++;
//         }

//         return true;
//     }
