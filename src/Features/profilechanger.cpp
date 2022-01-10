#include "profilechanger.h"

#include "../settings.h"
#include "../interfaces.h"

int Settings::ProfileChanger::coinID = 890;
int Settings::ProfileChanger::musicID = 10;
int Settings::ProfileChanger::compRank = 18;
int Settings::ProfileChanger::weaponStatus = 0;
int Settings::ProfileChanger::weaponRarity = 0;

void ProfileChanger::UpdateProfile()
{
        if (!engine->IsInGame())
                return;
        C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
        if (!localplayer)
                return;

        const auto local_index = localplayer->GetIndex();

        if (auto player_resource = *csPlayerResource)
        {
                player_resource->SetActiveCoinRank()[local_index] = Settings::ProfileChanger::coinID;
                switch (Settings::ProfileChanger::type)
                {
                case MusicType::CSGO:
                        player_resource->SetMusicID()[local_index] = 1;
                        break;
                case MusicType::CSGO2:
                        player_resource->SetMusicID()[local_index] = 2;
                        break;
                case MusicType::CRIMSON_ASSAULT:
                        player_resource->SetMusicID()[local_index] = 3;
                        break;
                case MusicType::SHARPENED:
                        player_resource->SetMusicID()[local_index] = 4;
                        break;
                case MusicType::INSURGENCY:
                        player_resource->SetMusicID()[local_index] = 5;
                        break;
                case MusicType::ADB:
                        player_resource->SetMusicID()[local_index] = 6;
                        break;
                case MusicType::HIGH_MOON:
                        player_resource->SetMusicID()[local_index] = 7;
                        break;
                case MusicType::DEATHS_HEAD_DEMOLITION:
                        player_resource->SetMusicID()[local_index] = 8;
                        break;
                case MusicType::DESERT_FIRE:
                        player_resource->SetMusicID()[local_index] = 9;
                        break;
                case MusicType::LNOE:
                        player_resource->SetMusicID()[local_index] = 10;
                        break;
                case MusicType::METAL:
                        player_resource->SetMusicID()[local_index] = 11;
                        break;
                case MusicType::ALL_I_WANT_FOR_CHRISTMAS:
                        player_resource->SetMusicID()[local_index] = 12;
                        break;
                case MusicType::ISORHYTHM:
                        player_resource->SetMusicID()[local_index] = 13;
                        break;
                case MusicType::FOR_NO_MANKIND:
                        player_resource->SetMusicID()[local_index] = 14;
                        break;
                case MusicType::HOTLINE_MIAMI:
                        player_resource->SetMusicID()[local_index] = 15;
                        break;
                case MusicType::TOTAL_DOMINATION:
                        player_resource->SetMusicID()[local_index] = 16;
                        break;
                case MusicType::THE_TALOS_PRINCIPLE:
                        player_resource->SetMusicID()[local_index] = 17;
                        break;
                case MusicType::BATTLEPACK:
                        player_resource->SetMusicID()[local_index] = 18;
                        break;
                case MusicType::MOLOTOV:
                        player_resource->SetMusicID()[local_index] = 19;
                        break;
                case MusicType::UBER_BLASTO_PHONE:
                        player_resource->SetMusicID()[local_index] = 20;
                        break;
                case MusicType::HAZARDOUS_ENVIRONMENTS:
                        player_resource->SetMusicID()[local_index] = 21;
                        break;
                case MusicType::II_HEADSHOT:
                        player_resource->SetMusicID()[local_index] = 22;
                        break;
                case MusicType::THE_8_BIT_KIT:
                        player_resource->SetMusicID()[local_index] = 23;
                        break;
                case MusicType::I_AM:
                        player_resource->SetMusicID()[local_index] = 24;
                        break;
                case MusicType::DIAMONDS:
                        player_resource->SetMusicID()[local_index] = 25;
                        break;
                case MusicType::INVASION:
                        player_resource->SetMusicID()[local_index] = 26;
                        break;
                case MusicType::LIONS_MOUTH:
                        player_resource->SetMusicID()[local_index] = 27;
                        break;
                case MusicType::SPONGE_FINGERZ:
                        player_resource->SetMusicID()[local_index] = 28;
                        break;
                case MusicType::DISGUSTING:
                        player_resource->SetMusicID()[local_index] = 29;
                        break;
                case MusicType::JAVA_HAVANA_FUNKALOO:
                        player_resource->SetMusicID()[local_index] = 30;
                        break;
                case MusicType::MOMENTS_CSGO:
                        player_resource->SetMusicID()[local_index] = 31;
                        break;
                case MusicType::AGGRESSIVE:
                        player_resource->SetMusicID()[local_index] = 32;
                        break;
                case MusicType::THE_GOOD_YOUTH:
                        player_resource->SetMusicID()[local_index] = 33;
                        break;
                case MusicType::FREE:
                        player_resource->SetMusicID()[local_index] = 34;
                        break;
                case MusicType::LIFES_NOT_OUT_TO_GET_YOU:
                        player_resource->SetMusicID()[local_index] = 35;
                        break;
                case MusicType::BACKBONE:
                        player_resource->SetMusicID()[local_index] = 36;
                        break;
                case MusicType::GLA:
                        player_resource->SetMusicID()[local_index] = 37;
                        break;
                case MusicType::III_ARENA:
                        player_resource->SetMusicID()[local_index] = 38;
                        break;
                case MusicType::EZ4ENCE:
                        player_resource->SetMusicID()[local_index] = 39;
                        break;
                }
                player_resource->SetCompetitiveRanking()[local_index] = Settings::ProfileChanger::compRank;
        }
}
