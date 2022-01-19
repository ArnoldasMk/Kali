#include "hooks.h"
#include <future>

#include "../interfaces.h"
#include "../settings.h"
#include "../Features/slowwalk.h"
#include "../Features/bhop.h"
#include "../Features/noduckcooldown.h"
#include "../Features/autostrafe.h"
#include "../Features/showranks.h"
#include "../Features/autodefuse.h"
#include "../Features/jumpthrow.h"
#include "../Features/grenadehelper.h"
#include "../Features/grenadeprediction.h"
#include "../Features/edgejump.h"
#include "../Features/autoblock.h"
#include "../Features/predictionsystem.h"
#include "../Features/ragebot.h"
#include "../Features/legitbot.h"
#include "../Features/triggerbot.h"
#include "../Features/autoknife.h"
#include "../Features/antiaim.h"
#include "../Features/fakelag.h"
#include "../Features/esp.h"
#include "../Features/tracereffect.h"
#include "../Features/nofall.h"
#include "../Features/ragdollgravity.h"
#include "../Features/lagcomp.h"
#include "../Features/fakeduck.h"
#include "../Features/silentWalk.h"
#include "../Features/quickswitch.h"
#include "../Features/memeangle.h"
#include "../Features/global.h"

bool CreateMove::sendPacket = true;
QAngle CreateMove::lastTickViewAngles = QAngle(0);

typedef bool (*CreateMoveFn)(void *, float, CUserCmd *);

bool Hooks::CreateMove(void *thisptr, float flInputSampleTime, CUserCmd *cmd)
{
	clientModeVMT->GetOriginalMethod<CreateMoveFn>(25)(thisptr, flInputSampleTime, cmd);

	if (cmd && cmd->command_number)
	{
		// Special thanks to Gre-- I mean Heep ( https://www.unknowncheats.me/forum/counterstrike-global-offensive/290258-updating-bsendpacket-linux.html )
		uintptr_t *rbp;

		asm volatile("mov %%rbp, %0"
				   : "=r"(rbp));
		bool *sendPacket = ((*(bool **)rbp) - (int)24);
		CreateMove::sendPacket = true;

		/* run code that affects movement before prediction */
		BHop::CreateMove(cmd);
		AutoStrafe::CreateMove(cmd);
		global::CreateMove(cmd);
		SilentWalk::CreateMove(cmd);
		NoDuckCooldown::CreateMove(cmd);
		ShowRanks::CreateMove(cmd);
		AutoDefuse::CreateMove(cmd);
		JumpThrow::CreateMove(cmd);
		GrenadeHelper::CreateMove(cmd);
		GrenadePrediction::CreateMove(cmd);
		EdgeJump::PrePredictionCreateMove(cmd);
		Autoblock::CreateMove(cmd);
		NoFall::PrePredictionCreateMove(cmd);
		PredictionSystem::StartPrediction(cmd);
		FakeLag::CreateMove(cmd);
		LagComp::CreateMove(cmd);
		Legitbot::CreateMove(cmd);
		Ragebot::CreateMove(cmd);
		Triggerbot::CreateMove(cmd);
		AutoKnife::CreateMove(cmd);
		memeangles::CreateMove(cmd);
		AntiAim::CreateMove(cmd);
		FakeDuck::CreateMove(cmd);
		SlowWalk::CreateMove(cmd);
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
