#include "ragdollgravity.h"


void RagdollGravity::CreateMove(ICvar* cvar)
{
    static auto ragdollGravity = cvar->FindVar("cl_ragdoll_gravity");
    //findVar("cl_ragdoll_gravity");
    ragdollGravity->SetValue(Settings::RagdollGravity::enabled ? -600 : 600);
    //ragdollGravity->setValue(config->visuals.inverseRagdollGravity ? -600 : 600);
}