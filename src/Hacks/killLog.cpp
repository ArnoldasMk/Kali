#include "killLog.h"

#include "../Utils/xorstring.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../ATGUI/atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

void KillLog::RenderWindow()
{
        if (!Settings::ShowKills::enabled)
                return;

        if (!UI::isVisible && !engine->IsInGame())
                return;
        if( Settings::UI::Windows::Kills::reload )
        {
                ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Kills::posX, Settings::UI::Windows::Kills::posY), ImGuiSetCond_Always);
                ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Kills::sizeX, Settings::UI::Windows::Kills::sizeY), ImGuiSetCond_Always);
                Settings::UI::Windows::Kills::reload = false;
        }
        else
        {
                ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Kills::posX, Settings::UI::Windows::Kills::posY), ImGuiSetCond_FirstUseEver);
                ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Kills::sizeX, Settings::UI::Windows::Kills::sizeY), ImGuiSetCond_FirstUseEver);
        }
        if (ImGui::Begin(XORSTR("Kill Log"), &Settings::ShowKills::enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar ))
        {

                ImVec2 temp = ImGui::GetWindowSize();
                Settings::UI::Windows::Kills::sizeX = (int)temp.x;
                Settings::UI::Windows::Kills::sizeY = (int)temp.y;
                temp = ImGui::GetWindowPos();
                Settings::UI::Windows::Kills::posX = (int)temp.x;
                Settings::UI::Windows::Kills::posY = (int)temp.y;

                ImGui::Columns(2);
                ImGui::Separator();

                ImGui::Text(XORSTR("Name"));
                ImGui::NextColumn();

                ImGui::Text(XORSTR("Damage"));
                ImGui::NextColumn();

                ImGui::Text(XORSTR("Resolver"));
                ImGui::NextColumn();

	        ImGui::Text(XORSTR("Misses"));
                ImGui::NextColumn();
//		int i;
//                for (std::string data : KillLog::kills)
//                {
//
//                        ImGui::Separator();
//			char *token = strtok("%s", "-");
//    			while (token != NULL)
//    			{
//			i++;
 //                       ImGui::Text("%s");
   //                     ImGui::NextColumn();
     //   		token = strtok(NULL, "-");
//
//		}

    //            ImGui::Columns(1);
  //              ImGui::Separator();
//
  //              ImGui::End();
//        	}
	}

}
