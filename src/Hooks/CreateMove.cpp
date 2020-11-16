#include "hooks.h"
#include <future>

#include "../interfaces.h"
#include "../settings.h"
#include "../Hacks/slowwalk.h"
#include "../Hacks/bhop.h"
#include "../Hacks/noduckcooldown.h"
#include "../Hacks/autostrafe.h"
#include "../Hacks/showranks.h"
#include "../Hacks/autodefuse.h"
#include "../Hacks/jumpthrow.h"
#include "../Hacks/grenadehelper.h"
#include "../Hacks/grenadeprediction.h"
#include "../Hacks/edgejump.h"
#include "../Hacks/autoblock.h"
#include "../Hacks/predictionsystem.h"
#include "../Hacks/ragebot.h"
#include "../Hacks/legitbot.h"
#include "../Hacks/triggerbot.h"
#include "../Hacks/autoknife.h"
#include "../Hacks/antiaim.h"
#include "../Hacks/fakelag.h"
#include "../Hacks/esp.h"
#include "../Hacks/tracereffect.h"
#include "../Hacks/nofall.h"
#include "../Hacks/ragdollgravity.h"
#include "../Hacks/lagcomp.h"
#include "../Hacks/fakeduck.h"
#include "../Hacks/Tickbase.h"
#include "../Hacks/silentWalk.h"
#include "../Hacks/quickswitch.h"
#include "../Hacks/fakewalk.h"
#include "../Hacks/memeangle.h"
bool CreateMove::sendPacket = true;
QAngle CreateMove::lastTickViewAngles = QAngle(0);

typedef bool (*CreateMoveFn) (void*, float, CUserCmd*);

bool Hooks::CreateMove(void* thisptr, float flInputSampleTime, CUserCmd* cmd)
{
	clientModeVMT->GetOriginalMethod<CreateMoveFn>(25)(thisptr, flInputSampleTime, cmd);

	if (cmd && cmd->command_number)
	{
        // Special thanks to Gre-- I mean Heep ( https://www.unknowncheats.me/forum/counterstrike-global-offensive/290258-updating-bsendpacket-linux.html )
        uintptr_t* rbp;
		
        asm volatile("mov %%rbp, %0" : "=r" (rbp));
        bool *sendPacket = ((*(bool **)rbp) - (int)24);
        CreateMove::sendPacket = true;

		/* run code that affects movement before prediction */
		BHop::CreateMove(cmd);
                AutoStrafe::CreateMove(cmd);
                SilentWalk::CreateMove(cmd);
		NoDuckCooldown::CreateMove(cmd);
		ShowRanks::CreateMove(cmd);
		AutoDefuse::CreateMove(cmd);
		JumpThrow::CreateMove(cmd);
		GrenadeHelper::CreateMove(cmd);
        GrenadePrediction::CreateMove( cmd );
        EdgeJump::PrePredictionCreateMove(cmd);
		Autoblock::CreateMove(cmd);
		NoFall::PrePredictionCreateMove(cmd);
		PredictionSystem::StartPrediction(cmd);
		FakeLag::CreateMove(cmd);
		FakeeWalk::CreateMove(cmd);
		LagComp::CreateMove(cmd);
		Legitbot::CreateMove(cmd);
		Ragebot::CreateMove(cmd);
		Tickbase::run(cmd, CreateMove::sendPacket);
		Triggerbot::CreateMove(cmd);
		AutoKnife::CreateMove(cmd);
        memeangles::CreateMove(cmd);
    	AntiAim::CreateMove(cmd);
		FakeDuck::CreateMove(cmd);
		FakeWalk::CreateMove(cmd);
		ESP::CreateMove(cmd);
		TracerEffect::CreateMove(cmd);
 		QuickSwitch::CreateMove(cmd);
		RagdollGravity::CreateMove(cvar);
		PredictionSystem::EndPrediction();
		
		EdgeJump::PostPredictionCreateMove(cmd);
		NoFall::PostPredictionCreateMove(cmd);

        *sendPacket = CreateMove::sendPacket;

        if (CreateMove::sendPacket)
            CreateMove::lastTickViewAngles = cmd->viewangles;
	}

	return false;
}

