#include "hvhtab.h"

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Hacks/valvedscheck.h"
#include "../../ImGUI/imgui_internal.h"

#pragma GCC diagnostic ignored "-Wformat-security"

void AntiAim::RenderTab()
{
    ImGui::Columns(2, nullptr, true);
    {
        ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), true);
        {
            ImGui::Text(XORSTR("AntiAim"));
            ImGui::BeginChild(XORSTR("##ANTIAIM"), ImVec2(0, 0), true);
            {
                ImGui::Columns(1);
                ImGui::Separator();
                ImGui::Checkbox(XORSTR("Rage AntiAim"), &Settings::AntiAim::RageAntiAim::enable);
                if (Settings::AntiAim::RageAntiAim::enable) { Settings::AntiAim::LegitAntiAim::enable = false; }
                if(Settings::AntiAim::RageAntiAim::enable) {
                    ImGui::Columns(1);
                    ImGui::Checkbox(XORSTR("Auto AntiAIm"), &Settings::AntiAim::HeadEdge::enabled);
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat(XORSTR("##EDGEDISTANCE"), &Settings::AntiAim::HeadEdge::distance, 20, 30, "Distance: %0.f");
                        ImGui::PopItemWidth();
                    }
                }

                ImGui::Columns(1);
                ImGui::Separator();
                ImGui::Checkbox(XORSTR("Legit AntiAim"), &Settings::AntiAim::LegitAntiAim::enable);
                if (Settings::AntiAim::LegitAntiAim::enable) 
                {
                    ImGui::Text("It can reveal you in overwatch Use in your own risk");
                    Settings::AntiAim::RageAntiAim::enable = false;

                }
                ImGui::Separator();
        
                ImGui::Columns(1);
                ImGui::Separator();
                ImGui::Text(XORSTR("Disable"));
                ImGui::Separator();
                ImGui::Checkbox(XORSTR("Knife"), &Settings::AntiAim::AutoDisable::knifeHeld);
                ImGui::Checkbox(XORSTR("No Enemy"), &Settings::AntiAim::AutoDisable::noEnemy);

                
                
                ImGui::Columns(1);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(210, 85));
                if (ImGui::BeginPopupModal(XORSTR("Error###UNTRUSTED_AA")))
                {
                    ImGui::Text(XORSTR("You cannot use this antiaim type on a VALVE server."));

                    ImGui::Checkbox(XORSTR("This is not a VALVE server"), &ValveDSCheck::forceUT);

                    if (ImGui::Button(XORSTR("OK")))
                        ImGui::CloseCurrentPopup();

                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();

                ImGui::EndChild();
            }
            ImGui::EndChild();
        }
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), true);
        {
            ImGui::Text(XORSTR("Resolver"));
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Resolve All"), &Settings::Resolver::resolveAll);
            ImGui::Separator();
            ImGui::Text(XORSTR("Movement"));
            ImGui::Checkbox(XORSTR("Auto Crouch"), &Settings::Legitbot::AutoCrouch::enabled);
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Angle Indicator"), &Settings::AngleIndicator::enabled);
            ImGui::Checkbox(XORSTR("LBY Breaker"), &Settings::AntiAim::LBYBreaker::enabled);
            if( Settings::AntiAim::LBYBreaker::enabled ){
                ImGui::SliderFloat(XORSTR("##LBYOFFSET"), &Settings::AntiAim::LBYBreaker::offset, 0, 360, "LBY Offset(from fake): %0.f");
            }
            ImGui::EndChild();
        }
    }
}
