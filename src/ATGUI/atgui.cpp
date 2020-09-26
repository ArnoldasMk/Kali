
#include "atgui.h"


#include "../Utils/draw.h"
#include "../interfaces.h"
#include "../settings.h"
#include "../Utils/xorstring.h"
#include "menu_font.h"

#include "Windows/colors.h"
#include "Windows/configs.h"
#include "Windows/main.h"
#include "Windows/playerlist.h"
#include "Windows/skinmodelchanger.h"
#include "Windows/showspectators.h"
#include "../Hacks/radar.h"
#include "../Hacks/antiaim.h"
#include "../Hacks/killLog.h"
bool UI::isVisible = false;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void UI::SwapWindow()
{
	if (UI::isVisible)
		return;

	if (engine->IsInGame())
		return;

    Draw::ImText( ImVec2( 4.f, 4.f ), ImColor( 255, 255, 255, 255 ), XORSTR( "MissedIT" ), nullptr, 0.0f, nullptr,
                  ImFontFlags_Shadow );
}

void UI::SetVisible(bool visible)
{
	UI::isVisible = visible;
	cvar->FindVar(XORSTR("cl_mouseenable"))->SetValue(!UI::isVisible);
}

bool UI::DrawImWatermark()
{
	if (UI::isVisible)
		return false;

	// if (engine->IsInGame())
	// 	return false;

    ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
    ImGui::SetNextWindowSize( ImVec2( 200, 0 ), ImGuiCond_Always );
    ImGui::SetNextWindowBgAlpha( 0.0f );
	ImGuiStyle& style = ImGui::GetStyle();
            style.WindowBorderSize = 0.0f;
    if ( ImGui::Begin( XORSTR("##mainFrame"), (bool*)false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize ) ) ;
    {
//		Draw::ImText( ImVec2( 20.f, 10.f ), ImColor( 255, 255, 255, 255 ), XORSTR( "MissedIT | FPS :" ), nullptr, 0.0f, nullptr,
//	                ImFontFlags_Outline );
//		int fps = static_cast< int >( 1.f / globalVars->frametime );
//		std::string fps_string = std::to_string(fps);
//		Draw::ImText( ImVec2( 140.f, 10.f ), ImColor( 255, 255, 255, 255 ), fps_string.c_str() , nullptr, 0.0f, nullptr,
  //                	ImFontFlags_Outline );
//                Draw::AddRectFilled(1653 + 73, 2, 1653 + 260, 30, ImColor(40, 40, 40, 225));
  //              Draw::AddRectFilled(1653 + 5 + 73, 1 + 5, 1653 + 255, 30 - 5, ImColor(10, 10, 10, 225));
    //            Draw::AddRect(1653 - 1 + 73, 1, 1653 + 261, 31, ImColor(200, 200, 200, 50));
      //          Draw::AddRect(1652 + 5 + 73, 1 + 5, 1653 + 256, 31 - 5, ImColor(200, 200, 200, 50));
        //        Draw::AddLine(1653 + 6 + 73, 1 + 5, 1653 + 254, 1 + 5, Settings::ESP::Chams::Arms::color.Color());
//int fps = static_cast< int >( 1.f / globalVars->frametime );
  //            std::string fps_string = std::to_string(fps);
//std::string name = "eyehook | " + fps_string + " fps | 39ms";
//std::string name = "eye     | " + fps_string + " fps | 39ms";

//Draw::AddText(1653 + 10 + 73, 11, name.c_str(), ImColor( 255, 255, 255, 255 ) );
//Draw::AddText(1653 + 10 + 73 + 21, 11, "hook", ImColor( 255, 166, 14, 255 ) );
		
		ImGui::End();
	}

	return true;
}

void UI::SetupWindows()
{
	ImVec2 size = ImGui::GetWindowSize();
	if (UI::isVisible)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2( (size.x - 960)/2,(size.x - 645)/2));
		//MainHeader::RenderWindow();
		Main::RenderWindow();
		ImGui::PopStyleVar();
		Configs::RenderWindow();
		// Colors::RenderWindow();
	}

	PlayerList::RenderWindow();
	ShowSpectators::RenderWindow();
        KillLog::RenderWindow();

	Radar::RenderWindow();
}

void UI::angleIndicator()
{

	ImGui::SetNextWindowPos( ImVec2( 0, 400 ), ImGuiCond_Always );
    ImGui::SetNextWindowSize( ImVec2( 100, 0 ), ImGuiCond_Always );

	ImGui::SetNextWindowBgAlpha( 0.0f );
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;
	if ( ImGui::Begin( XORSTR("##indecator"), (bool*)false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize ) ) ;
    {
int width, height;
engine->GetScreenSize(width, height);

		if (AntiAim::ManualAntiAim::alignBack){
        Draw::ImText( ImVec2( 500, 500 ), ImColor( 255, 255, 255, 255 ), XORSTR( "v" ), nullptr, 0.0f, nullptr, ImFontFlags_Shadow );
		}
    else if (AntiAim::ManualAntiAim::alignLeft)
        Draw::ImText( ImVec2( width / 2 - 10, height / 2 ), ImColor( 54, 154, 255, 255 ), XORSTR( "<" ), nullptr, 0.0f, nullptr, ImFontFlags_Shadow );
    else if (AntiAim::ManualAntiAim::alignRight)
        Draw::ImText( ImVec2( width / 2 + 10, height / 2), ImColor( 54, 154, 255, 255 ), XORSTR( ">" ), nullptr, 0.0f, nullptr, ImFontFlags_Shadow );
		
		ImGui::End();
	}
}
