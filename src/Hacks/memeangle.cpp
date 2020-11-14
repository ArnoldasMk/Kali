#include "memeangle.h"


void memeangles::FrameStageNotify(ClientFrameStage_t stage)
{
/*
if (stage == ClientFrameStage_t::FRAME_RENDER_START){
    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    auto animstate = localplayer->GetAnimState();
	if (!animstate)
		return;
    bool real_server_update = false;
	if (localplayer->GetSimulationTime() != real_simulation_time)
	{
		real_server_update = true;
		real_simulation_time = localplayer->GetSimulationTime();
	}
	    CUtlVector<AnimationLayer>* layers = localplayer->GetAnimOverlay();

	    layers->operator[](3).m_flWeight = 0.0f;
            layers->operator[](3).m_flCycle = 0.0f;
            layers->operator[](12).m_flWeight = 0.0f;
	if (real_server_update){
	abs_angles = animstate->GoalFeetYaw;

	}

}
*/
}
