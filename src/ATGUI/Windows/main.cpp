
#include "main.h"

#include "../../settings.h"
#include "../Windows/configs.h"
#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"
#include "../Tabs/legitbottab.h"
#include "../Tabs/visualstab.h"
#include "../Tabs/ragebottab.h"
#include "../Tabs/hvhtab.h"
#include "../Tabs/misctab.h"
#include "../Tabs/playerinfo.h"
#include "../Tabs/visualstab.h"
#include "../Tabs/skinstab.h"
#include "../Tabs/modelstab.h"
#include "../Tabs/skinsandmodel.h"
#include "../menu_font.h"
#include <vector>
bool MainMenu::showWindow = true;

static int tabs = 0;
static int subtabs = 0;

enum
{
	AimbotTab,
	RageTab,
	HvHTab,
	Visuals,
	SkinChangerTab,
	MiscTab,
	PlayerInfo,
};

enum
{
	ESP,
	Local,
	Materials,
	Menu,
};

const char *TabsNames[] = {"Aimbot", "Rage", "HvH", "Visuals", "Skin Changer", "Misc", "Player List"};

std::initializer_list<const char *> subtabAimbot = {};
std::initializer_list<const char *> subtabRage = {};
std::initializer_list<const char *> subtabHvHTab = {};
std::initializer_list<const char *> subtabVisuals = {"ESP", "Local", "Materials", "Menu"};
std::initializer_list<const char *> subtabSkinChanger = {"Skins", "Models"};
std::initializer_list<const char *> subtabMisc = {};
std::initializer_list<const char *> subtabPlayerInfo = {};

std::vector<std::initializer_list<const char *>> allsubtabs{
    subtabAimbot,
    subtabRage,
    subtabHvHTab,
    subtabVisuals,
    subtabSkinChanger,
    subtabMisc,
    subtabPlayerInfo,
};

static ImVec2 pos;
ImDrawList *draw;

static void BgDecorations()
{
	ImGuiIO &io = ImGui::GetIO();

	auto info = io.FontDefault;
	pos = ImGui::GetWindowPos();
	draw = ImGui::GetWindowDrawList();

	// Background
	draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1128, pos.y + 865), ImColor(4, 3, 5), 0);
	draw->AddRectFilled(ImVec2(pos.x, pos.y + 50), ImVec2(pos.x + 1128, pos.y + 52), ImColor(2, 6, 11));
	// Footer Border
	draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1128, pos.y + 50), ImColor(2, 6, 11), 0, 3);
	// Footer
	draw->AddRectFilled(ImVec2(pos.x, pos.y + 835), ImVec2(pos.x + 1128, pos.y + 865), ImColor(2, 6, 11), 0, 4 | 8);
	draw->AddText(info, 13.f, ImVec2(pos.x + 12, pos.y + 845), ImColor(255, 255, 255), "Kali");
	//  Side Panel Bg
	draw->AddRectFilled(ImVec2(pos.x, pos.y + 55), ImVec2(pos.x + 160, pos.y + 835), ImColor(2, 19, 30, 150));
	ImGui::SetCursorPos(ImVec2(10, 10));
}

static void Tabs()
{
	static int tab_sizes = IM_ARRAYSIZE(TabsNames);
	static int tab_gaps = 120;
	static int tabY = 10;
	static int tabX;
	tabX = 60;
	for (int i = 0; i < tab_sizes; i++)
	{
		ImGui::SetCursorPos(ImVec2(tabX, tabY));
		if (ImGui::Tab(TabsNames[i], i == tabs))
		{
			tabs = i;
			subtabs = 0;
		}

		tabX += tab_gaps;
	}
}

static void SubTabs()
{
	static int tab_gaps = 40;
	static int tabY;
	static int tabX = 0;
	tabY = 53;
	static int subtabs_size = allsubtabs.size();
	for (int i = 0; i < subtabs_size; i++)
	{
		if (i == tabs)
		{
			int j = 0;
			for (auto _subtabname : allsubtabs[i])
			{
				ImGui::SetCursorPos(ImVec2(tabX, tabY));
				if (ImGui::Sub(_subtabname, j == subtabs))
					subtabs = j;
				j++;
				tabY += tab_gaps;
			}
			break;
		}
	}
}
static void VisualsTab()
{
	switch (subtabs)
	{
	case ESP:
		VisualsESP::RenderMainMenu(pos, draw, subtabs);
		break;
	case Local:
		VisualsLocal::RenderMainMenu(pos, draw, subtabs);
		break;
	case Materials:
		VisualsMaterialConfig::RenderMainMenu(pos, draw, subtabs);
		break;
	case Menu:
		// VisualsMenu::RenderMainMenu(pos, draw, subtabs);
		break;
	}
}

void MainMenu::Render()
{
	ImVec2 size = ImGui::GetWindowSize();
	size = ImVec2((size.x - 1128) / 2, (size.y - 865) / 2);

	ImGui::SetNextWindowPos(ImVec2(size.x, size.y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(1128, 865), ImGuiCond_Once);
	ImGui::SetNextWindowBgAlpha(0.0f);
	if (ImGui::Begin(XORSTR("##Menu"), &MainMenu::showWindow, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/ | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiConfigFlags_NoMouseCursorChange))
	{
		BgDecorations();
		Tabs();
		SubTabs();

		// // cvar->ConsoleDPrintf(XORSTR("Selected tab : %d\n"), tabs);
		switch (tabs)
		{
		case AimbotTab:
			draw->AddRectFilled(ImVec2(pos.x, pos.y + 52), ImVec2(pos.x + 160, pos.y + 836), ImColor(4, 3, 5));
			Legitbot::RenderMainMenu(pos, draw, subtabs);
			break;
		case RageTab:
			draw->AddRectFilled(ImVec2(pos.x, pos.y + 52), ImVec2(pos.x + 160, pos.y + 836), ImColor(4, 3, 5));
			RagebotTab::RenderMainMenu(pos, draw, subtabs);
			break;
		case HvHTab:
			draw->AddRectFilled(ImVec2(pos.x, pos.y + 52), ImVec2(pos.x + 160, pos.y + 836), ImColor(4, 3, 5));
			HvH::RenderMainMenu(pos, draw, subtabs);
			break;
		case Visuals:
			VisualsTab();
			break;
		case SkinChangerTab:
			SkinsAndModel::RenderMainMenu(pos, draw, subtabs);
			break;
		case MiscTab:
			draw->AddRectFilled(ImVec2(pos.x, pos.y + 52), ImVec2(pos.x + 160, pos.y + 836), ImColor(4, 3, 5));
			MiscCustomizations::RenderMainMenu(pos, draw, subtabs);
			break;
		case PlayerInfo:
			draw->AddRectFilled(ImVec2(pos.x, pos.y + 52), ImVec2(pos.x + 160, pos.y + 836), ImColor(4, 3, 5));
			PlayerInfoTab::RenderMainMenu(pos, draw, subtabs);
		default:
			break;
		}
		ImGui::End();
	}
}
