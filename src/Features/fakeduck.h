#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/CViewRender.h"

namespace FakeDuck
{

//Hooks
void CreateMove(CUserCmd *cmd);
void OverrideView(CViewSetup *pSetup);

}; // namespace FakeDuck