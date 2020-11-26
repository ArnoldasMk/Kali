/*#include "hooks.h"
#include "../interfaces.h"
#include "../Hacks/fakelag.h"
#include "../hooker.h"
#include "../Utils/xorstring.h"

typedef bool (*WriteUserCmdDeltFn) (void* thisptr, int slot, bf_write *buf, int from, int to, bool isnewcommand );

bool Hooks::WriteUsercmdDeltaToBuffer(void* thisptr, int slot, bf_write *buf, int from, int to, bool isnewcommand){

    static auto funcAdd = clientVMT->GetOriginalMethod<WriteUserCmdDeltFn>(24);
    funcAdd(thisptr,slot, buf, from, to, isnewcommand );
if (FakeLag::shift <= 0)
     return funcAdd(thisptr, slot, buf, from, to, isnewcommand );
   if (from != -1)
      return true;

    int* numBackupCommands = (int*)(reinterpret_cast <uintptr_t> (buf) - 0x30);
    int* numNewCommands = (int*)(reinterpret_cast <uintptr_t> (buf) - 0x2C);
    int32_t newcommands = *numNewCommands;

    int nextcommmand; //= memory->clientState->lastOutgoingCommand + memory->clientState->chokedCommands + 1;
    int totalcommands = std::min(FakeLag::shift, 14);
    FakeLag::shift = 0;
    from = -1;
    *numNewCommands = totalcommands;
    *numBackupCommands = 0;
    for (to = nextcommmand - newcommands + 1; to <= nextcommmand; to++)
    {
       if (!(funcAdd(thisptr, slot, buf, from, to, isnewcommand )))
            return false;

        from = to;
    }

    CUserCmd lastRealCmd = inputSystem->GetUserCmd(slot, from);
        cvar->ConsoleDPrintf(XORSTR("ye\n"));
    CUserCmd fromcmd;
        fromcmd = lastRealCmd;
   CUserCmd tocmd = fromcmd;
    tocmd.tick_count += 200;
    tocmd.command_number++;

    for (int i = newcommands; i <= totalcommands; i++)
    {
//        WriteUserCmd(buf, &tocmd, &fromcmd);
        fromcmd = tocmd;
        tocmd.command_number++;
        tocmd.tick_count++;
    }

    return true;
}
*/
