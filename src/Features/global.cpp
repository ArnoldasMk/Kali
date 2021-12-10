#include "global.h"

void global::CreateMove(CUserCmd* cmd)
{
global::local = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
global::tickcount = cmd->tick_count;

}
