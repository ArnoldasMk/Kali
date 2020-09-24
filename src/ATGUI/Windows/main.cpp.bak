#include "main.h"

#include "../../settings.h"
#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"
#include "../menu_font.h"
#include "../Tabs/legitbottab.h"
#include "../Tabs/ragebottab.h"
#include "../Tabs/antiaimtab.h"
#include "../Tabs/misctab.h"
#include "../Tabs/triggerbottab.h"
#include "../Tabs/visualstab.h"
#include "../Tabs/skinstab.h"
#include "../Tabs/modelstab.h"
#include "../Tabs/skinsandmodel.h"

#include "colors.h"
#include "configs.h"

bool Main::showWindow = true;

static void Buttons()
{
	ImVec2 size =  ImGui::GetWindowSize();
	size = ImVec2( (size.x - Settings::UI::Windows::Main::sizeX)/ 2, (size.y - Settings::UI::Windows::Main::sizeY) / 2);
	
	ImGui::SetNextWindowPos(ImVec2(0, (ImGui::GetWindowSize().y / 2) - 20), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(size.x - 20,30), ImGuiCond_Once );

	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0,0,0,0);

	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
						Settings::UI::mainColor.Color().Value.x ,
						Settings::UI::mainColor.Color().Value.y ,
						Settings::UI::mainColor.Color().Value.z ,
						Settings::UI::mainColor.Color().Value.w
				);

	if (ImGui::Begin(XORSTR("##BUTTONS"), &Main::showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiConfigFlags_NoMouseCursorChange ) )
	{
		ImGui::Columns(1);
		{			
			ImGui::PushItemWidth(-1);
			if (ImGui::Button(XORSTR("Config"), ImVec2( ImGui::GetWindowSize().x, 50) ) )
				Configs::showWindow = !Configs::showWindow;

			ImGui::PopItemWidth();
		}
	}
	ImGui::End();

	ImGui::GetStyle().Colors[ImGuiCol_Button] = Settings::UI::accentColor.Color();
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = Settings::UI::bodyColor.Color();

}

void Main::RenderWindow()
{
//ImGuiIO& io = ImGui::GetIO();
//ImFont* font0 =  ImGui::GetIO().Fonts->AddFontDefault();
//ImFont* font1 = io.Fonts->AddFontFromFileTTF("/usr/share/fonts/TTF/menu_font.ttf", 12);
//io.Fonts->GetTexDataAsAlpha8();
//ImGui::PopFont();
	if (!Main::showWindow)
	{
		Settings::UI::Windows::Main::open = false;
		return;
	} 

	ImVec2 size =  ImGui::GetWindowSize();
	size = ImVec2( (size.x - Settings::UI::Windows::Main::sizeX)/ 2, (size.y - Settings::UI::Windows::Main::sizeY) / 2);
	if( Settings::UI::Windows::Main::reload )
	{
		ImGui::SetNextWindowPos(ImVec2(size.x, size.y), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiCond_Once);
		Main::showWindow = Settings::UI::Windows::Main::open;
		Settings::UI::Windows::Main::reload = false;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(size.x, size.y), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiCond_Once);
	}

	static int page = 0;

	ImVec2 temp = ImGui::GetWindowSize();
	
	if (ImGui::Begin(XORSTR("##MissedIt"), &Main::showWindow, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/ | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoTitleBar | ImGuiConfigFlags_NoMouseCursorChange ) )
	{
		Settings::UI::Windows::Main::open = true;

		Settings::UI::Windows::Main::sizeX = (int)temp.x;
		Settings::UI::Windows::Main::sizeY = (int)temp.y;
		temp = ImGui::GetWindowPos();
		Settings::UI::Windows::Main::posX = (int)temp.x;
		Settings::UI::Windows::Main::posY = (int)temp.y;
	//	ImFont* menu_font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(menu_font_compressed_data,menu_font_compressed_size,/*Size*/ 25);
          //      ImGui::PushFont(menu_font);

		// const char* tabs[] = {
		// 		"Legit Bot",
		// 		"Rage Bot(Beta)",
		// 		"Anti Aim",
		// 		"Visuals",
		// 		"Skin/Model",
		// 		"Misc",
		// };
		const char* tabs[] = {
				"A",
				"B",
				"C",
				"D",
				"E",
				"F",
		};
		ImGui::Columns(2, nullptr, false);
		{
			float ButtonsXSize = ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs)-9;
			ImGui::SetColumnOffset(1, ButtonsXSize);

			for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
			{
				int distance = i == page ? 0 : i > page ? i - page : page - i;

				ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
					Settings::UI::mainColor.Color().Value.x - (distance * 0.035f),
					Settings::UI::mainColor.Color().Value.y - (distance * 0.035f),
					Settings::UI::mainColor.Color().Value.z - (distance * 0.035f),
					Settings::UI::mainColor.Color().Value.w
				);
//ImGui::PushFont(font1);
				if (ImGui::Button(tabs[i], ImVec2( ButtonsXSize, 0)))
					page = i;

				ImGui::GetStyle().Colors[ImGuiCol_Button] = Settings::UI::accentColor.Color();

				// if (i < IM_ARRAYSIZE(tabs) - 1)
				// {
				// 	ImGui::SameLine();
				// 	ImGui::Dummy(ImVec2(-1,-1));
				// }
//ImGui::PushFont(font0);
				
		}
//ImGui::PopFont();
//ImGui::PushFont(font0);
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), false);
			{
				switch (page)
				{
				case 0:
					Legitbot::RenderTab();
					break;
				case 1:
					RagebotTab::RenderTab();
					break;
				case 2:
					HvH::RenderTab();
					break;
				case 3:
					Visuals::RenderTab();
					break;
				case 4:
					SkinsAndModel::RenderTab();
					break;
				// case 5:
				// 	Models::RenderTab();
				// 	break;
				case 5:
					Misc::RenderTab();
					break;

				}
			}
			ImGui::EndChild();
		}
		
		
	}
	ImGui::EndColumns();
	ImGui::End();
	
	Buttons();
}
