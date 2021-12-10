#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Features/snipercrosshair.h"

typedef bool (*ShouldDrawCrosshairFn) (void*);

bool Hooks::ShouldDrawCrosshair(void* thisptr)
{
    bool ret = clientModeVMT->GetOriginalMethod<ShouldDrawCrosshairFn>(29)(thisptr);
	if( SniperCrosshair::DrawCrosshair() )
		return false;

	return ret;
}
