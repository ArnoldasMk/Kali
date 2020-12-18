#include "hooks.h"

#include "../interfaces.h"

#include "../Hacks/customglow.h"
#include "../Hacks/skinchanger.h"
#include "../Hacks/noflash.h"
#include "../Hacks/view.h"
#include "../Hacks/resolver.h"
#include "../Hacks/resolverAP.h"
#include "../Hacks/skybox.h"
#include "../Hacks/asuswalls.h"
#include "../Hacks/nosmoke.h"
#include "../Hacks/thirdperson.h"
#include "../settings.h"
#include "../Hacks/lagcomp.h"
#include "../Hacks/antiaim.h"
#include "../Hacks/svcheats.h"
#include "../Hacks/memeangle.h"
#include "../Hacks/animfix.h"


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
