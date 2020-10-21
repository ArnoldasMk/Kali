#include "memeangle.h"
void memeangles::FrameStageNotify(ClientFrameStage_t stage)
{
        if (!engine->IsInGame())
                return;


        C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
        if (!localplayer || !localplayer->GetAlive())
                return;

        if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
        {

                bool Pitch = Settings::Resolver::rPitch;
                int maxClient = engine->GetMaxClients();
                for (int i = 1; i < maxClient; ++i)
                {
                        //indx = i;
                        C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

                        if (!player 
                        || player == localplayer 
                        || player->GetDormant() 
                        || !player->GetAlive() 
                        || player->GetImmune()
                        || !(Entity::IsTeamMate(player, localplayer)))
                                continue;
//			player->GetAnimState()->yaw = AntiAim::fakeAngle.y;

		}
	}
}
