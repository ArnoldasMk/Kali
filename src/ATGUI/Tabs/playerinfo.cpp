#include "playerinfo.h"
#include <sstream>

#include "../../interfaces.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"

#include "../../Features/legitbot.h"
#include "../../Features/esp.h"
#include "../../Features/resolver.h"
#include "../../Features/clantagchanger.h"
#include "../../Features/namechanger.h"
#pragma GCC diagnostic ignored "-Wformat-security"

void PlayerInfoTab::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
		ImGui::Text(XORSTR("hello"));
	}
	ImGui::EndGroup();
}
