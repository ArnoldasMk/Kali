#include "showspectators.h"

#include "../Utils/xorstring.h"
#include "../settings.h"
#include "../interfaces.h"
#include "../ATGUI/atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

std::list<int> ShowSpectators::GetObservervators(int playerId)
{
	std::list<int> list;

	if (!engine->IsInGame())
		return list;

	C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(playerId);
	if (!player)
		return list;

	if (!player->GetAlive())
	{
		C_BasePlayer* observerTarget = (C_BasePlayer*) entityList->GetClientEntityFromHandle(player->GetObserverTarget());
		if (!observerTarget)
			return list;

		player = observerTarget;
	}

	for (int i = 1; i < engine->GetMaxClients(); i++)
	{
		C_BasePlayer* pPlayer = (C_BasePlayer*) entityList->GetClientEntity(i);
		if (!pPlayer)
			continue;

		if (pPlayer->GetDormant() || pPlayer->GetAlive())
			continue;

		C_BasePlayer* target = (C_BasePlayer*) entityList->GetClientEntityFromHandle(pPlayer->GetObserverTarget());
		if (player != target)
			continue;

		list.push_back(i);
	}

	return list;
}

void ShowSpectators::RenderWindow()
{
	if (!Settings::ShowSpectators::enabled)
		return;

	if (!UI::isVisible && !engine->IsInGame())
		return;
	if( Settings::UI::Windows::Spectators::reload )
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Spectators::posX, Settings::UI::Windows::Spectators::posY), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Spectators::sizeX, Settings::UI::Windows::Spectators::sizeY), ImGuiSetCond_Always);
		Settings::UI::Windows::Spectators::reload = false;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Spectators::posX, Settings::UI::Windows::Spectators::posY), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Spectators::sizeX, Settings::UI::Windows::Spectators::sizeY), ImGuiSetCond_FirstUseEver);
	}
	if (ImGui::Begin(XORSTR("Spectators"), &Settings::ShowSpectators::enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar ))
	{
		ImVec2 temp = ImGui::GetWindowSize();
		Settings::UI::Windows::Spectators::sizeX = (int)temp.x;
		Settings::UI::Windows::Spectators::sizeY = (int)temp.y;
		temp = ImGui::GetWindowPos();
		Settings::UI::Windows::Spectators::posX = (int)temp.x;
		Settings::UI::Windows::Spectators::posY = (int)temp.y;

		ImGui::Columns(2);
		ImGui::Separator();

		ImGui::Text(XORSTR("Name"));
		ImGui::NextColumn();

		ImGui::Text(XORSTR("Mode"));
		ImGui::NextColumn();

		for (int playerId : ShowSpectators::GetObservervators(engine->GetLocalPlayer()))
		{
			if (playerId == engine->GetLocalPlayer())
				continue;

			C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(playerId);

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(playerId, &entityInformation);

			if (entityInformation.fakeplayer)
				continue;

			ImGui::Separator();

			ImGui::Text("%s", entityInformation.name);
			ImGui::NextColumn();

			switch (*player->GetObserverMode())
			{
				case ObserverMode_t::OBS_MODE_IN_EYE:
					ImGui::Text(XORSTR("Perspective"));
					break;
				case ObserverMode_t::OBS_MODE_CHASE:
					ImGui::Text(XORSTR("3rd person"));
					break;
				case ObserverMode_t::OBS_MODE_ROAMING:
					ImGui::Text(XORSTR("Free look"));
					break;
				case ObserverMode_t::OBS_MODE_DEATHCAM:
					ImGui::Text(XORSTR("Deathcam"));
					break;
				case ObserverMode_t::OBS_MODE_FREEZECAM:
					ImGui::Text(XORSTR("Freezecam"));
					break;
				case ObserverMode_t::OBS_MODE_FIXED:
					ImGui::Text(XORSTR("Fixed"));
					break;
				default:
					break;
			}
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::Separator();

		ImGui::End();
	}
}
