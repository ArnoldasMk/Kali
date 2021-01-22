#include <string>
#include "voterevealer.h"
#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../settings.h"
#include "../interfaces.h"

void VoteRevealer::FireGameEvent(IGameEvent *event){

if (!Settings::voterevealer::enabled)
	return;

if (strstr(event->GetName(), XORSTR("vote_cast"))){ 
	int vote_player_id = event->GetInt(XORSTR("entityid"));
	IEngineClient::player_info_t playerInfo;
        engine->GetPlayerInfo( vote_player_id, &playerInfo );
	int option = event->GetInt("vote_option");
	std::string votestring = "say ";
	votestring += playerInfo.name;
        cvar->ConsoleDPrintf(std::string(playerInfo.name).c_str());
		if (option == 0){
	votestring += " Voted yes";
        cvar->ConsoleDPrintf(XORSTR(" Voted yes\n"));
		}else{
	        votestring += " Voted no";

        cvar->ConsoleDPrintf(XORSTR(" Voted no\n"));		
		}
//engine->ExecuteClientCmd(votestring.c_str());
}
	}
