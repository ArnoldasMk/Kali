
#include "atgui.h"

#include "particles.h"

#include "../SDK/IVModelRender.h"
#include "../Utils/draw.h"
#include "../interfaces.h"
#include "../settings.h"
#include "../Utils/xorstring.h"
#include "menu_font.h"
#include "Windows/colors.h"
#include "Windows/configs.h"
#include "Windows/main.h"
#include "Windows/skinmodelchanger.h"
#include "Windows/showspectators.h"
#include "../Features/radar.h"
#include "../Features/antiaim.h"
bool UI::isVisible = false;

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*_ARR)))

void UI::SwapWindow()
{
	if (UI::isVisible)
		return;

	if (engine->IsInGame())
		return;

	Draw::ImText(ImVec2(4.f, 4.f), ImColor(255, 255, 255, 255), XORSTR("Kali"), nullptr, 0.0f, nullptr,
			   ImFontFlags_Shadow);
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

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGuiStyle &style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;
	if (ImGui::Begin(XORSTR("##mainFrame"), (bool *)false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize));
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2((size.x - 1128) / 2, (size.x - 865) / 2));
		//ImGui::ShowStyleEditor();
		//if (Settings::UI::particles)
		//dot_draw();
		//MainHeader::RenderWindow();
		//auto blur = material->FindMaterial("dev/motion_blur", TEXTURE_GROUP_MODEL);
		//blur->DrawScreenSpaceRectangle(blur, 0, 0, 1920, 1080, 0, 0, 1920, 1080, 1920, 1080, nullptr, 1, 1);

		MainMenu::Render();
		ImGui::PopStyleVar();
		Configs::RenderWindow();
		// Colors::RenderWindow();
	}
	ShowSpectators::RenderWindow();
	Radar::RenderWindow();
}

void UI::angleIndicator()
{
	ImGui::SetNextWindowPos(ImVec2(0, 400), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(100, 0), ImGuiCond_Always);

	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGuiStyle &style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;
	if (ImGui::Begin(XORSTR("##indecator"), (bool *)false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize))
		;
	{
		int width, height;
		engine->GetScreenSize(width, height);

		if (AntiAim::ManualAntiAim::alignBack)
		{
			Draw::ImText(ImVec2(500, 500), ImColor(255, 255, 255, 255), XORSTR("v"), nullptr, 0.0f, nullptr, ImFontFlags_Shadow);
		}
		else if (AntiAim::ManualAntiAim::alignLeft)
			Draw::ImText(ImVec2(width / 2 - 10, height / 2), ImColor(54, 154, 255, 255), XORSTR("<"), nullptr, 0.0f, nullptr, ImFontFlags_Shadow);
		else if (AntiAim::ManualAntiAim::alignRight)
			Draw::ImText(ImVec2(width / 2 + 10, height / 2), ImColor(54, 154, 255, 255), XORSTR(">"), nullptr, 0.0f, nullptr, ImFontFlags_Shadow);

		ImGui::End();
	}
}
