#include "tooltip.h"
#include "../ImGUI/imgui.h"
#include "../Utils/xorstring.h"

void ToolTip::Show(const char* tooltip, bool IsItemHovered)
{
    if(!IsItemHovered)
        return;
    // auto massage = tooltip;
    ImGui::SetNextWindowSize(ImVec2(), ImGuiCond_Always );
    ImGui::SetTooltip(XORSTR("%c"), tooltip);
}