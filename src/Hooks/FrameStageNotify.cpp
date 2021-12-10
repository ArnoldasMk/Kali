#include "hooks.h"

#include "../interfaces.h"

#include "../Features/customglow.h"
#include "../Features/skinchanger.h"
#include "../Features/noflash.h"
#include "../Features/view.h"
#include "../Features/resolver.h"
#include "../Features/resolverAP.h"
#include "../Features/skybox.h"
#include "../Features/asuswalls.h"
#include "../Features/nosmoke.h"
#include "../Features/thirdperson.h"
#include "../settings.h"
#include "../Features/lagcomp.h"
#include "../Features/antiaim.h"
#include "../Features/svcheats.h"
#include "../Features/memeangle.h"
#include "../Features/animfix.h"


typedef void (*FrameStageNotifyFn) (void*, ClientFrameStage_t);

void Hooks::FrameStageNotify(void* thisptr, ClientFrameStage_t stage)
{
 	AntiAim::FrameStageNotify(stage);
	CustomGlow::FrameStageNotify(stage);
	SkinChanger::FrameStageNotifyModels(stage);
	SkinChanger::FrameStageNotifySkins(stage);
	Noflash::FrameStageNotify(stage);
	View::FrameStageNotify(stage);
        memeangles::FrameStageNotify(stage);
	Resolver::FrameStageNotify(stage);
	AnimFix::FrameStageNotify(stage);
	//ResolverAP::FrameStageNotify(stage);
	SkyBox::FrameStageNotify(stage);
	ASUSWalls::FrameStageNotify(stage);
	NoSmoke::FrameStageNotify(stage);
	ThirdPerson::FrameStageNotify(stage);
	SvCheats::FrameStageNotify(stage);
	if (SkinChanger::forceFullUpdate)
	{
		GetLocalClient(-1)->m_nDeltaTick = -1;
		SkinChanger::forceFullUpdate = false;
	}

	clientVMT->GetOriginalMethod<FrameStageNotifyFn>(37)(thisptr, stage);
	
	View::PostFrameStageNotify(stage);
    //Resolver::PostFrameStageNotify(stage);
	ResolverAP::PostFrameStageNotify(stage);

}
