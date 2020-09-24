#include "spammer.h"

#include <sstream>

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "esp.h"
#include "../ImGUI/imgui_internal.h"
#include "../settings.h"
#include "../interfaces.h"

std::vector<int> killedPlayerQueue;

void Spammer::BeginFrame(float frameTime)
{
	if (!engine->IsInGame())
		return;

	// Grab the current time in milliseconds
	long currentTime_ms = Util::GetEpochTime();
	static long timeStamp = currentTime_ms;

	if (currentTime_ms - timeStamp < 850)
		return;

	// Kill spammer
	if (Settings::Spammer::KillSpammer::enabled && killedPlayerQueue.size() > 0)
	{
		IEngineClient::player_info_t playerInfo;
		engine->GetPlayerInfo(killedPlayerQueue[0], &playerInfo);

		// Prepare dead player's nickname without ';' & '"' characters
		// as they might cause user to execute a command.
		std::string dead_player_name = std::string(playerInfo.name);
		dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), ';'), dead_player_name.end());
		dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), '"'), dead_player_name.end());

		// Remove end line character
		dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), '\n'), dead_player_name.end());

		// Construct a command with our message
		std::ostringstream str;
		str << (Settings::Spammer::KillSpammer::sayTeam ? XORSTR("say_team") : XORSTR("say"));
		std::string message = Settings::Spammer::KillSpammer::messages[std::rand() % Settings::Spammer::KillSpammer::messages.size()];
		str << " \"" << Util::ReplaceString(message, XORSTR("$nick"), dead_player_name) << "\"";

		// Execute our constructed command
		engine->ExecuteClientCmd(str.str().c_str());

		// Remove the first element from the vector
		killedPlayerQueue.erase(killedPlayerQueue.begin(), killedPlayerQueue.begin() + 1);

		return;
	}

	if (Settings::Spammer::RadioSpammer::enabled)
	{
		const char* radioCommands[] = {
				"coverme",
				"takepoint",
				"holdpos",
				"regroup",
				"followme",
				"takingfire",
				"go",
				"fallback",
				"sticktog",
				"report",
				"roger",
				"enemyspot",
				"needbackup",
				"sectorclear",
				"inposition",
				"reportingin",
				"getout",
				"negative",
				"enemydown",
		};

		engine->ClientCmd_Unrestricted(radioCommands[std::rand() % IM_ARRAYSIZE(radioCommands)]);
	}

	if (Settings::Spammer::type == SpammerType::SPAMMER_NORMAL)
	{
		if (Settings::Spammer::NormalSpammer::messages.empty())
			return;

		// Grab a random message string
		std::string message = Settings::Spammer::NormalSpammer::messages[std::rand() % Settings::Spammer::NormalSpammer::messages.size()];

		// Construct a command with our message
		std::ostringstream str;
		str << (Settings::Spammer::say_team ? XORSTR("say_team") : XORSTR("say")) << " ";
		str << message;

		// Execute our constructed command
		engine->ExecuteClientCmd(str.str().c_str());
	}
	else if (Settings::Spammer::type == SpammerType::SPAMMER_POSITIONS)
	{
		C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
		static int lastId = 1;

		for (int i = lastId; i < engine->GetMaxClients(); i++)
		{
			C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

			lastId++;
			if (lastId == engine->GetMaxClients())
				lastId = 1;

			if (!player
				|| player->GetDormant()
				|| !player->GetAlive())
				continue;

			if (Settings::Spammer::PositionSpammer::team == 0 && !Entity::IsTeamMate(player, localplayer))
				continue;

			if (Settings::Spammer::PositionSpammer::team == 1 && Entity::IsTeamMate(player, localplayer))
				continue;

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(player->GetActiveWeapon());

			// Prepare player's nickname without ';' & '"' characters
			// as they might cause user to execute a command.
			std::string playerName = std::string(entityInformation.name);
			playerName.erase(std::remove(playerName.begin(), playerName.end(), ';'), playerName.end());
			playerName.erase(std::remove(playerName.begin(), playerName.end(), '"'), playerName.end());

			// Remove end line character
			playerName.erase(std::remove(playerName.begin(), playerName.end(), '\n'), playerName.end());

			// Construct a command with our message
			std::ostringstream str;
			str << (Settings::Spammer::say_team ? XORSTR("say_team") : XORSTR("say")) << " \"";

			if (Settings::Spammer::PositionSpammer::showName)
				str << playerName << " | ";

			if (Settings::Spammer::PositionSpammer::showWeapon)
				str << Util::Items::GetItemDisplayName(*activeWeapon->GetItemDefinitionIndex()) << " | ";

			if (Settings::Spammer::PositionSpammer::showRank)
				str << ESP::ranks[*(*csPlayerResource)->GetCompetitiveRanking(i)] << " | ";

			if (Settings::Spammer::PositionSpammer::showWins)
				str << *(*csPlayerResource)->GetCompetitiveWins(i) << XORSTR(" wins | ");

			if (Settings::Spammer::PositionSpammer::showHealth)
				str << player->GetHealth() << XORSTR("HP | ");

			if (Settings::Spammer::PositionSpammer::showMoney)
				str << "$" << player->GetMoney() << XORSTR(" | ");

			if (Settings::Spammer::PositionSpammer::showLastplace)
				str << player->GetLastPlaceName();

			str << "\"";

			// Execute our constructed command
			engine->ExecuteClientCmd(str.str().c_str());

			break;
		}
	}

	// Update the time stamp
	timeStamp = currentTime_ms;
}

void Spammer::FireGameEvent(IGameEvent* event)
{
	if (!Settings::Spammer::KillSpammer::enabled)
		return;

	if (!engine->IsInGame())
		return;

	if (strcmp(event->GetName(), XORSTR("player_death")) != 0)
		return;

	int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
	int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

	// Make sure it's not a suicide.x
	if (attacker_id == deadPlayer_id)
		return;

	// Make sure we're the one who killed someone...
	if (attacker_id != engine->GetLocalPlayer())
		return;

	killedPlayerQueue.push_back(deadPlayer_id);
}
