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
    const char* yType[] = {
        "NONE",
        "Static",
        "Jitter",
        "Random",
    };

    const char* RageAntiAimType[] = {
        "NONE",
        "Fake Arround Real(Beta)",
        "Real Arround Fake",
    };
    ImGui::Columns(1, nullptr, false); // Pick Rage Anti Aim type
    {
        ImGui::PushItemWidth(-1);
        ImGui::Combo(XORSTR("##RageAntiAimType"), (int*)&Settings::AntiAim::RageAntiAim::Type, RageAntiAimType, IM_ARRAYSIZE(RageAntiAimType));
        ImGui::PopItemWidth();
    }

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

    if ( Settings::AntiAim::RageAntiAim::Type == RageAntiAimType::RealArroundFake )
    {
        // Fake Percentage
        ImGui::Columns(1);
        {
            ImGui::PushItemWidth(-1);
            if(Settings::AntiAim::Yaw::typeFake == AntiAimFakeType_y::Jitter)
                ImGui::SliderFloat(XORSTR("##FakeJitterPercentage"), &Settings::AntiAim::RageAntiAim::JitterPercent, 0, 100, "Fake Jitter Ammount : %.0f perent");

            if (Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Static)
            {
                ImGui::SliderFloat(XORSTR("##RealPercentage"), &Settings::AntiAim::RageAntiAim::AntiAImPercent, 0, 100, "Real Ammount : %.0f percent");
                ImGui::Text(XORSTR("Invert Key"));
                ImGui::SameLine();
                UI::KeyBindButton(&Settings::AntiAim::RageAntiAim::InvertKey);      
            }
            else if (Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Jitter)      
                ImGui::SliderFloat(XORSTR("##RealJitterPercentage"), &Settings::AntiAim::RageAntiAim::AntiAImPercent, 0, 100, "Real Jitter Ammount : %.0f percent");
            
            ImGui::PopItemWidth();
        }    
    }
    
    else if (Settings::AntiAim::RageAntiAim::Type == RageAntiAimType::FakeArroundReal)
    {
        // Real Percentage
        ImGui::Columns(1);
        {
            ImGui::PushItemWidth(-1);
            if(Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Jitter)
                ImGui::SliderFloat(XORSTR("##RealJitterPercentage"), &Settings::AntiAim::RageAntiAim::JitterPercent, 0, 100, "Real Jitter Ammount : %.0f perent");
        
            
            if (Settings::AntiAim::Yaw::typeFake == AntiAimFakeType_y::Static)
            {
                ImGui::SliderFloat(XORSTR("##FakePercentage"), &Settings::AntiAim::RageAntiAim::AntiAImPercent, 0, 100, "Fake Ammount : %.0f percent");
                ImGui::Text(XORSTR("Invert Key"));
                ImGui::SameLine();
                UI::KeyBindButton(&Settings::AntiAim::RageAntiAim::InvertKey);      
            }
            else if (Settings::AntiAim::Yaw::typeFake == AntiAimFakeType_y::Jitter)  
                      ImGui::SliderFloat(XORSTR("##FakeJitterPercentage"), &Settings::AntiAim::RageAntiAim::AntiAImPercent, 0, 100, "Fake Jitter Ammount : %.0f percent");
         
            ImGui::PopItemWidth();
        }
    }
    

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::Columns(1, nullptr, false);
    
    ImGui::Checkbox(XORSTR("At the Target(alfa)"), &Settings::AntiAim::RageAntiAim::atTheTarget);
    

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::Columns(1);
    ImGui::Text(XORSTR("Edging(Alfa)"));
    
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
    ImGui::Spacing(); ImGui::Spacing();
    ImGui::Columns(1);
    ImGui::Checkbox(XORSTR("##Manual Anti Aim"), &Settings::AntiAim::ManualAntiAim::Enable);
    ImGui::SameLine();
    ImGui::Text(XORSTR("Manuan AntiAim"));

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
}

static void LegitAntiAim()
{
    /*
    * part where legit anti aim ui constructed
    */
   ImGui::Spacing();
    ImGui::Columns(1, nullptr, false);
    ImGui::Text(XORSTR("InvertKey"));
    ImGui::SameLine();
    UI::KeyBindButton(&Settings::AntiAim::LegitAntiAim::InvertKey); 
    ImGui::Spacing(); ImGui::Spacing(); 
    ImGui::PushItemWidth(-1); 
    ImGui::SliderFloat(XORSTR("##RealPercentage"), &Settings::AntiAim::LegitAntiAim::RealPercentage, 0, 100, "Real Percent : %.0f percent" );  
    ImGui::Spacing();
    ImGui::SliderFloat(XORSTR("##INCROUTCHREALPERCENTAGE"), &Settings::AntiAim::LegitAntiAim::RealPercentageInCroutch, 0, 100, "Real Percent In Croutch : %.0f percent" ); 
    ImGui::PopItemWidth();
    /*
    * End of legit antiaim
    */
}

static void LagacyAntiAim()
{
    ImGui::SliderFloat(XORSTR("##LBYOFFSET"), &Settings::AntiAim::LBYBreaker::offset, 0, 360, "LBY Offset(from fake): %0.f");
}
void HvH::RenderTab()
{
    
    static char* AntiAimType[] = {
        "Legit AntiAIm",
        "Rage AntiAIm",
        "LBY Breaker",
    }; 
    
    ImGui::Spacing();

    ImGui::Columns(2, nullptr, false);
    {
        switch (Settings::AntiAim::Type::antiaimType)
        {
        case AntiAimType::LegitAntiAim:
           if ( ImGui::Checkbox(XORSTR("##EnableLegitAim"), &Settings::AntiAim::LegitAntiAim::enable) )
                Settings::AntiAim::RageAntiAim::enable = false;
            break;
        case AntiAimType::RageAntiAim:
            if ( ImGui::Checkbox(XORSTR("##EnableRageAntiAim"), &Settings::AntiAim::RageAntiAim::enable) )
                Settings::AntiAim::LegitAntiAim::enable = false;
            break;
        case AntiAimType::Lagacy:
            ImGui::Checkbox(XORSTR("##ENABLELbyBreaker"), &Settings::AntiAim::LBYBreaker::enabled);
            break;
        default:
            break;
        }
            
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        ImGui::Combo(XORSTR("##AntiAimType"), (int*)&Settings::AntiAim::Type::antiaimType, AntiAimType, IM_ARRAYSIZE(AntiAimType));
        ImGui::PopItemWidth();
        ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), false);
        {
            switch (Settings::AntiAim::Type::antiaimType)
            {
            case AntiAimType::RageAntiAim:
                RageAntiAIm();
                break;
            case AntiAimType::LegitAntiAim:
                LegitAntiAim();
                break;
            case AntiAimType::Lagacy:
                LagacyAntiAim();
                break;
            default:
                break;
            }
        }
        ImGui::EndChild();
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), false);
        {
            ImGui::Checkbox(XORSTR("Angle Indicator"), &Settings::AngleIndicator::enabled);
            ImGui::EndChild();
        }
    }
}
