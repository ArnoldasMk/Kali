#include "skinsandmodel.h"

#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui.h"
#include "../../settings.h"
#include "skinstab.h"
#include "modelstab.h"

const char *skinORmodel[] = {
    "Skins",
    "Models"};

void SkinsAndModel::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{

	ImGui::SetCursorPos(ImVec2(180, 65));
	ImGui::BeginGroup();
	{
		if (sideTabIndex == (int) SkinAndModel::Skins) {
			Skins::RenderTab();
		} else if (sideTabIndex == (int) SkinAndModel::Model) {
			Models::RenderTab();
		}
	}
	ImGui::EndGroup();
}