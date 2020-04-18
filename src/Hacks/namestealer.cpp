#include "namestealer.h"

#include "namechanger.h"
#include "../Utils/util.h"
#include "../Utils/xorstring.h"
#include "../settings.h"
#include "../interfaces.h"

int NameStealer::entityId = -1;

void NameStealer::BeginFrame(float frameTime)
{
	if (!Settings::NameStealer::enabled)
		return;

	if (!engine->IsInGame())
		return;

	long currentTime_ms = Util::GetEpochTime();
	static long timeStamp = currentTime_ms;

	if (currentTime_ms - timeStamp < 350)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	while (entityId < engine->GetMaxClients())
	{
		entityId++;

		if (entityId >= engine->GetMaxClients())
			entityId = 1;

		if (entityId == 0)
		{
			NameChanger::SetName(XORSTR("\n\xAD\xAD\xAD"));

			timeStamp = currentTime_ms;

			break;
		}

		if ((*csPlayerResource) && (*csPlayerResource)->GetConnected(entityId))
		{
			// TODO: Replace with IsTeamMate().
			if (Settings::NameStealer::team == 0 && (*csPlayerResource)->GetTeam(entityId) != localplayer->GetTeam())
				break;

			if (Settings::NameStealer::team == 1 && (*csPlayerResource)->GetTeam(entityId) == localplayer->GetTeam())
				break;

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(entityId, &entityInformation);

			if (entityInformation.ishltv)
				break;

			NameChanger::SetName(Util::PadStringRight(entityInformation.name, strlen(entityInformation.name) + 1));

			timeStamp = currentTime_ms;
		}

		break;
	}
}

void NameStealer::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) != 0 && strcmp(event->GetName(), XORSTR("cs_game_disconnected")) != 0)
		return;

	if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
		return;

	entityId = -1;
}
