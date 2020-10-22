#include "memeangle.h"

bool fresh_tick(){

}

void memeangles::FrameStageNotify(ClientFrameStage_t stage)
{
/*        if (!engine->IsInGame())
                return;


        C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
        if (!localplayer || !localplayer->GetAlive())
                return;

        if (stage == ClientFrameStage_t::FRAME_RENDER_START)
        {

	static float proper_abs = localplayer->GetAnimState()->goalFeetYaw;
	static std::array<float, 24> sent_pose_params = localplayer->pose_params_();
	if (fresh_tick()) // Only update animations each tick, though we are doing this each frame.
	{
		std::memcpy(backup_layers, localplayer->get_anim_overlays(), (sizeof(c_animation_layer) * localplayer->get_number_of_anim_overlays()));
		localplayer->animations(true); //just does stuff like set m_bClientSideAnimation and m_iLastAnimUpdateFrameCount
		localplayer->update_state(localplayer->anim_state() g_client->view_angles);
		if (packet_sent)
		{
			proper_abs = localplayer->anim_state()->abs_yaw;
			sent_pose_params = localplayer->pose_params_();
		}
	}
	localplayer->animations(false);
	localplayer->set_abs_angles(vec_3d(0, proper_abs, 0));
	localplayer->anim_state()->magic_fraction = 0.f; // Lol.
	std::memcpy(localplayer->get_anim_overlays(), backup_layers, (sizeof(c_animation_layer) * localplayer->get_number_of_anim_overlays()));
	localplayer->pose_params_() = sent_pose_params;
}
*/
}
