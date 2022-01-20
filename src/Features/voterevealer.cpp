#include "voterevealer.h"
#include "../Utils/xorstring.h"
// #include "../SDK/IEngineClient.h"
#include "../settings.h"
#include "../interfaces.h"

void VoteRevealer::FireGameEvent(IGameEvent *event){
	if (!Settings::VoteRevealer::enabled)
		return;
	if (!strstr(event->GetName(), XORSTR("vote_cast")))
		return;

	cvar->ConsoleDPrintf("vote started");

	// int vote_player_id = event->GetInt(XORSTR("entityid"));

	IEngineClient::player_info_t playerInfo;
    engine->GetPlayerInfo(event->GetInt(XORSTR("entityid")), &playerInfo);
	cvar->ConsoleDPrintf("%s voted %s", XORSTR(playerInfo.name), XORSTR(event->GetInt("vote_option") ? "yes" : "no"));
}
