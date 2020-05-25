#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wformat-security"

#include "antiaimtab.h"

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Hacks/valvedscheck.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"



static void RageAntiAIm()
{
    // const char* yTypes[] = {
    //         "NONE", "MAX_DELTA_LEFT", "MAX_DELTA_RIGHT", "MAX_DELTA_FLIPPER", "MAX_DELTA_LBY_AVOID"
    // };

    const char* yType[] = {
        "NONE",
        "Static",
        "Jitter",
    };

    // const char* xTypes[] = {
    //         "UP", "DOWN", "DANCE", "FRONT", // safe
    //         "FAKE UP", "FAKE DOWN", "LISP DOWN", "ANGEL DOWN", "ANGEL UP" // untrusted
    // };

    if ( ImGui::Checkbox(XORSTR("Enable"), &Settings::AntiAim::RageAntiAim::enable) )
        Settings::AntiAim::LegitAntiAim::enable = false;
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    {
        ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
        ImGui::Text(XORSTR("Yaw Fake"));
        ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
        ImGui::Text(XORSTR("Yaw Actual"));
    }
    ImGui::NextColumn();
    {
        ImGui::PushItemWidth(-1);
        ImGui::Combo(XORSTR("##YFAKETYPE"), (int*)&Settings::AntiAim::Yaw::typeFake, yType, IM_ARRAYSIZE(yType));
        ImGui::Combo(XORSTR("##YACTUALTYPE"), (int*)& Settings::AntiAim::Yaw::typeReal, yType, IM_ARRAYSIZE(yType));
        ImGui::PopItemWidth();
    }
    // Fake Percentage
    ImGui::Columns(1);
    ImGui::PushItemWidth(-1);
    
    if(Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Jitter)
        ImGui::SliderFloat(XORSTR("##RealJitterPercentage"), &Settings::AntiAim::RageAntiAim::RealJitterPercent, 10, 100, "Real Jitter Ammount : %.0f perent");
    
    ImGui::SliderFloat(XORSTR("##FakePercentage"), &Settings::AntiAim::RageAntiAim::FakePercent, 10, 100, "Fake Ammount : %.0f percent");
    if (Settings::AntiAim::Yaw::typeFake == AntiAimFakeType_y::Static)
    {
        ImGui::Text(XORSTR("Invert Key"));
        ImGui::SameLine();
        UI::KeyBindButton(&Settings::AntiAim::RageAntiAim::InvertKey);      
    }        
         
    ImGui::PopItemWidth();

    ImGui::Checkbox(XORSTR("At the Target(alfa)"), &Settings::AntiAim::RageAntiAim::atTheTarget);            
    // ImGui::Columns(1);
    // ImGui::Separator();
    // ImGui::Text(XORSTR("Disable"));
    // ImGui::Separator();
    // ImGui::Checkbox(XORSTR("Knife"), &Settings::AntiAim::AutoDisable::knifeHeld);
    // ImGui::Checkbox(XORSTR("No Enemy"), &Settings::AntiAim::AutoDisable::noEnemy);

    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::Text(XORSTR("Edging(Alfa)"));
    ImGui::Separator();
    
    ImGui::Columns(2, nullptr, false);
    {
        ImGui::Checkbox(XORSTR("Enabled"), &Settings::AntiAim::HeadEdge::enabled);
    }
    ImGui::NextColumn();
    {
        ImGui::PushItemWidth(-1);
        ImGui::SliderFloat(XORSTR("##EDGEDISTANCE"), &Settings::AntiAim::HeadEdge::distance, 20, 30, "Distance: %0.0f");
        ImGui::PopItemWidth();
    }

    /*
    ** Starting of Manual anti aim
    */
    ImGui::Columns(1);
                
    ImGui::Separator();
    ImGui::Checkbox(XORSTR("##Manual Anti Aim"), &Settings::AntiAim::ManualAntiAim::Enable);
    ImGui::SameLine();
    ImGui::Text(XORSTR("Manuan AntiAim"));                
    ImGui::Separator();

    //For player to move right
    ImGui::Text("Align Right");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::RightButton);
    ImGui::PopItemWidth();

    // For player Move back
    ImGui::Text("Align Back");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::backButton);
    ImGui::PopItemWidth();

    //For player to move left
    ImGui::Text("Align Left");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::LeftButton);
    ImGui::PopItemWidth();
    // END

    /*
    ** Implementiong The secondary features with AntiAim
    */
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
}

static void LegitAntiAim()
{
    /*
    * part where legit anti aim ui constructed
    */
    ImGui::Columns(1, nullptr, true);
    if ( ImGui::Checkbox(XORSTR("Enable"), &Settings::AntiAim::LegitAntiAim::enable) )
        Settings::AntiAim::RageAntiAim::enable = false;
    ImGui::Text(XORSTR("InvertKey"));
    ImGui::SameLine();
    UI::KeyBindButton(&Settings::AntiAim::LegitAntiAim::InvertKey);   
    /*
    * End of legit antiaim
    */
}
void HvH::RenderTab()
{
    
    static char* AntiAimType[] = {
        "Legit AntiAIm",
        "Rage AntiAIm",
        "Lagacy",
    }; 

    ImGui::Columns(2, nullptr, false);
    {
        ImGui::PushItemWidth(-1);
        ImGui::Combo(XORSTR("##AntiAimType"), (int*)&Settings::AntiAim::Type::antiaimType, AntiAimType, IM_ARRAYSIZE(AntiAimType));
        ImGui::PopItemWidth();
        ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), true);
        {
                
            if ( Settings::AntiAim::Type::antiaimType == AntiAimType::RageAntiAim )
                RageAntiAIm();
            else if ( Settings::AntiAim::Type::antiaimType == AntiAimType::LegitAntiAim )
                LegitAntiAim();
        }
        ImGui::EndChild();
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), true);
        {
            ImGui::Text(XORSTR("Movement"));
            ImGui::Checkbox(XORSTR("Auto Crouch"), &Settings::Ragebot::AutoCrouch::enable);
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
