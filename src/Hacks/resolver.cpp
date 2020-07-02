#include "resolver.h"

#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"
#include "../interfaces.h"
#include "../settings.h"
#include "antiaim.h"

std::vector<int64_t> Resolver::Players = {};
std::vector<std::pair<C_BasePlayer *, QAngle>> player_data_Nimbus;

static float NormalizeAsYaw(float flAngle)
{
	if (flAngle > 180.f || flAngle < -180.f)
	{
		auto revolutions = round(abs(flAngle / 360.f));

		if (flAngle < 0.f)
			flAngle += 360.f * revolutions;
		else
			flAngle -= 360.f * revolutions;
	}

	return flAngle;
}

void Resolver::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!engine->IsInGame() || !Settings::Resolver::resolveAll)
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (int i = engine->GetMaxClients(); i > 1; i--)
		{
			C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

			if (!player 
			|| player == localplayer 
			|| player->GetDormant() 
			|| !player->GetAlive() 
			|| player->GetImmune() 
			|| Entity::IsTeamMate(player, localplayer))
				continue;

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			if (!Settings::Resolver::resolveAll && std::find(Resolver::Players.begin(), Resolver::Players.end(), entityInformation.xuid) == Resolver::Players.end())
				continue;

			player_data_Nimbus.push_back(std::pair<C_BasePlayer *, QAngle>(player, *player->GetEyeAngles()));

			/*
			cvar->ConsoleColorPrintf(ColorRGBA(64, 0, 255, 255), XORSTR("\n[Nimbus] "));
			cvar->ConsoleDPrintf("Debug log here!");
			*/

			// Tanner is a sex bomb, also thank you Stacker for helping us out!
			float lbyDelta = fabsf(NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y));

			if (lbyDelta < 35)
				return;

			if (player->GetEyeAngles()->x > 87.f)
			{
				static float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);

				if (player->GetVelocity().Length() < 10.0f)
				{
					player->GetAnimState()->goalFeetYaw = trueDelta <= 0
															  ? player->GetEyeAngles()->y + fabs(AntiAim::GetMaxDelta(player->GetAnimState()) * 0.99f)
															  : fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.99f) - player->GetEyeAngles()->y;
				}
				else
				{
					player->GetAnimState()->goalFeetYaw = trueDelta <= 0
															  ? player->GetEyeAngles()->y + fabs(AntiAim::GetMaxDelta(player->GetAnimState()) * 0.2f)
															  : fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.2f) - player->GetEyeAngles()->y;
				}
			}
			else
				player->GetEyeAngles()->y += *player->GetLowerBodyYawTarget();
		}
	}
	else if (stage == ClientFrameStage_t::FRAME_RENDER_END)
	{
		for (unsigned long i = 0; i < player_data_Nimbus.size(); i++)
		{
			std::pair<C_BasePlayer *, QAngle> player_aa_data = player_data_Nimbus[i];
			*player_aa_data.first->GetEyeAngles() = player_aa_data.second;
		}

		player_data_Nimbus.clear();
	}
}

void Resolver::FireGameEvent(IGameEvent *event)
{
	if (!engine->IsInGame() || !Settings::Resolver::resolveAll)
		return;

	if (!event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) != 0 && strcmp(event->GetName(), XORSTR("cs_game_disconnected")) != 0)
		return;

	if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
		return;

	Resolver::Players.clear();
}
