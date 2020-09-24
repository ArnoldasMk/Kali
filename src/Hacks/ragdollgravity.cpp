#include "ragdollgravity.h"


void RagdollGravity::CreateMove(ICvar* cvar)
{
    cvar->FindVar("cl_ragdoll_gravity")->SetValue(Settings::RagdollGravity::enabled ? -400 : 400);
}