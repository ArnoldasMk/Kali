
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
//ImGui::ShowStyleEditor();

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
