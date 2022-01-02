
#include "main.h"

#include "../../settings.h"
#include "../Windows/configs.h"
#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"
#include "../Tabs/legitbottab.h"
#include "../Tabs/visualstab.h"
#include "../Tabs/ragebottab.h"
#include "../Tabs/antiaimtab.h"
#include "../Tabs/misctab.h"
#include "../Tabs/playerinfo.h"
#include "../Tabs/triggerbottab.h"
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
	AntiAimTab,
	Visuals,
	SkinChangerTab,
	MiscTab,
	PlayerInfo,
	Dev,
};

enum
{
	LegitbotTab,
	RageTab,
	TriggerbotTab,
};

enum
{
	Enemy,
	Allies,
	Local,
	Materials,
	Menu,
};
enum
{
	Customizations,
};

const char *TabsNames[] = {"Aimbot", "Anti Aim", "Visuals", "Skin Changer", "Misc", "Player List"};

std::initializer_list<const char *> subtabAimbot = {"Legitbot", "Ragebot", "Triggerbot"};
std::initializer_list<const char *> subtabAntiAim = {};
std::initializer_list<const char *> subtabVisuals = {"Enemy", "Allies", "Local", "Materials", "Menu"};
std::initializer_list<const char *> subtabSkinChanger = {"Skins", "Models"};
std::initializer_list<const char *> subtabMisc = {"Custom"};
std::initializer_list<const char *> subtabPlayerInfo = {};
std::initializer_list<const char *> subtabDev = {"Options"};

std::vector<std::initializer_list<const char *>> allsubtabs{
    subtabAimbot,
    subtabAntiAim,
    subtabVisuals,
    subtabSkinChanger,
    subtabMisc,
    subtabPlayerInfo,
    subtabDev,
};

static ImVec2 pos;
ImDrawList *draw;

static void BgDecorations()
{
	ImGuiIO &io = ImGui::GetIO();

	auto info = io.FontDefault;
	pos = ImGui::GetWindowPos();
	draw = ImGui::GetWindowDrawList();

	// 645
	// Background
	draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1128, pos.y + 865), ImColor(4, 3, 5), 0);	 // ImColor(8, 8, 8, 200), 0);
																					 // Header
	draw->AddRectFilled(ImVec2(pos.x, pos.y + 50), ImVec2(pos.x + 1128, pos.y + 52), ImColor(2, 6, 11)); // ImColor(220, 60, 40));
	// Footer Border
	draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 1128, pos.y + 50), ImColor(2, 6, 11), 0, 3); // ImColor(187, 37, 38), 0, 3);
	// Footer
	draw->AddRectFilled(ImVec2(pos.x, pos.y + 835), ImVec2(pos.x + 1128, pos.y + 865), ImColor(2, 6, 11), 0, 4 | 8); // ImColor(187, 37, 38), 0, 4 | 8);

	draw->AddText(info, 13.f, ImVec2(pos.x + 12, pos.y + 845), ImColor(255, 255, 255), "Kali");
	// draw->AddText(info, 13.f, ImVec2(pos.x + 100, pos.y + 625), ImColor(255, 255, 255), "Counter-Strike: Global Offensive");
	// draw->AddText(info, 13.f, ImVec2(pos.x + 860, pos.y + 625), ImColor(255, 255, 255), "Kali");
	//
	//  Side Panel Bg
	draw->AddRectFilled(ImVec2(pos.x, pos.y + 55), ImVec2(pos.x + 160, pos.y + 835), ImColor(2, 19, 30, 150)); // mColor(0, 0, 0, 150));
	ImGui::SetCursorPos(ImVec2(10, 10));
	//
	// draw->AddRectFilled(ImVec2(pos.x + 595, pos.y + 15), ImVec2(pos.x + 785, pos.y + 35), ImColor(220, 220, 220), 5);
	// draw->AddText(info, 13.f, ImVec2(pos.x + 600, pos.y + 18), ImColor(136, 136, 134), u8"���� search features �� ���");
	//
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

static void aimbotTab()
{
	switch (subtabs)
	{
	case LegitbotTab:
		Legitbot::RenderMainMenu(pos, draw, subtabs);
		break;
	case RageTab:
		RagebotTab::RenderMainMenu(pos, draw, subtabs);
		break;
	default:
		Triggerbot::RenderMainMenu(pos, draw, subtabs);
		break;
	}
}

static void VisualsTab()
{
	switch (subtabs)
	{
	case Enemy:
		VisualsEnemy::RenderMainMenu(pos, draw, subtabs);
		break;
	case Allies:
		VisualsGood::RenderMainMenu(pos, draw, subtabs);
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

static void MiscellaneousTab()
{
	switch (subtabs)
	{
	case Customizations:
		MiscCustomizations::RenderMainMenu(pos, draw, subtabs);
		break;
		/*
		case Allies:
			VisualsGood::RenderMainMenu(pos, draw, subtabs);
			break;
		case Local:
			VisualsLocal::RenderMainMenu(pos, draw, subtabs);
			break;
			*/
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
			aimbotTab();
			break;
		case AntiAimTab:
			draw->AddRectFilled(ImVec2(pos.x, pos.y + 52), ImVec2(pos.x + 160, pos.y + 836), ImColor(4, 3, 5)); // ImColor(0, 0, 0, 150));
			AntiAim::RenderMainMenu(pos, draw, subtabs);
			break;
		case Visuals:
			VisualsTab();
			break;
		case SkinChangerTab:
			SkinsAndModel::RenderMainMenu(pos, draw, subtabs);
			break;
		case MiscTab:
			MiscellaneousTab();
			break;
		case PlayerInfo:
			draw->AddRectFilled(ImVec2(pos.x, pos.y + 52), ImVec2(pos.x + 160, pos.y + 832), ImColor(4, 3, 5)); // ImColor(0, 0, 0, 150));
			PlayerInfoTab::RenderMainMenu(pos, draw, subtabs);
		default:
			break;
		}
		ImGui::End();
	}
}
