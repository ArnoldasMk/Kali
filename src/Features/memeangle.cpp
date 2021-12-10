#include "memeangle.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#define BONE_USED_BY_ANYTHING           0x0007FF00

int count;
QAngle viewangle;
float real_abs;
void memeangles::CreateMove(CUserCmd* cmd)
{
//count = cmd->tick_count;
//viewangle = cmd->viewangles;
}

void memeangles::FrameStageNotify(ClientFrameStage_t stage)
{
return;
if (input->m_fCameraInThirdPerson){
			C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
                        auto state = localplayer->GetAnimState();
                        auto layers = localplayer->GetAnimOverlay();
                        if (state && layers) {
                                auto fresh_tick = []() -> bool {
                                        static int old_tick = count;
                                        if (old_tick != count) {
                                                old_tick = count;
                                                return true;
                                        }
                                        return false;
                                };
                                static float proper_abs = state->goalFeetYaw;
                                //static std::array<float, 24> sent_pose_params = csgo::local_player->pose_param();
                                static AnimationLayer backup_layers[15];
                                if (!backup_layers)
                                        std::memcpy(backup_layers, layers, (sizeof(AnimationLayer) * 15));

                               if (fresh_tick())
                                {
                                        std::memcpy(backup_layers, layers, (sizeof(AnimationLayer) * 15));
					localplayer->ClientAnimations(true);
					auto player_model_time = reinterpret_cast<int*>(state + 112);
                                        localplayer->updateClientAnimation();
                                        if (state)
                                                player_model_time = &globalVars->framecount - 1;
                                        if (CreateMove::sendPacket)
                                        {
                                                proper_abs = state->goalFeetYaw;
                                                real_abs = proper_abs;
                                        }
                                }
                                localplayer->ClientAnimations(false);
				const Vector mame (0, proper_abs, 0);
                                localplayer->SetAbsOrigin(&mame); 
                                std::memcpy(layers, backup_layers, (sizeof(AnimationLayer) * 15));
				localplayer->SetupBones(nullptr, 128, BONE_USED_BY_ANYTHING, globalVars->curtime);

}
}
}
