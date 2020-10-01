#include "svcheats.h"

#include "../interfaces.h"
#include "../settings.h"
#include "../Utils/xorstring.h"

void SvCheats::FrameStageNotify(ClientFrameStage_t stage)
{
    if (!Settings::SvCheats::enabled)
        return;

    if (!engine->IsInGame())
        return;

    static ConVar* svcheats = cvar->FindVar("sv_cheats");

    static ConVar* fullbright = cvar->FindVar("mat_fullbright");
    static ConVar* fogoverride = cvar->FindVar("fog_override");
    static ConVar* impacts = cvar->FindVar("sv_showimpacts");

    fullbright->SetValue(Settings::SvCheats::bright::enabled);
    fogoverride->SetValue(Settings::SvCheats::fog::enabled);
    impacts->SetValue(Settings::SvCheats::impacts::enabled);

    //if(Settings::SvCheats::nospread::enabled){ //meme
// }

    if(Settings::SvCheats::gravity::enabled) {
        static ConVar* invertedragdoll = cvar->FindVar("cl_ragdoll_gravity");
        invertedragdoll->flags &= ~FCVAR_CHEAT;
        invertedragdoll->SetValue(Settings::SvCheats::gravity::amount);
    } else {
        static ConVar* invertedragdoll = cvar->FindVar("cl_ragdoll_gravity");
        invertedragdoll->flags &= ~FCVAR_CHEAT;
        invertedragdoll->SetValue(800);
    }

    if(Settings::SvCheats::viewmodel::enabled) {
        static	ConVar* minspec = cvar->FindVar(XORSTR("sv_competitive_minspec"));
        minspec->fnChangeCallback = 0;
        minspec->SetValue(0);
    } else {
        static	ConVar* minspec = cvar->FindVar(XORSTR("sv_competitive_minspec"));
        minspec->fnChangeCallback = 0;
        minspec->SetValue(1);
    }
    if(Settings::SvCheats::viewmodel::enabled) {
        static ConVar* viewmodelfov = cvar->FindVar("viewmodel_fov");
        static ConVar* viewmodelx = cvar->FindVar("viewmodel_offset_x");
        static ConVar* viewmodely = cvar->FindVar("viewmodel_offset_y");
        static ConVar* viewmodelz = cvar->FindVar("viewmodel_offset_z");
        static ConVar* aspect = cvar->FindVar("r_aspectratio");
        float ratio = (Settings::SvCheats::aspect::var * 0.1) / 2;

        viewmodelfov->SetValue(Settings::SvCheats::viewmodel::fov);
        viewmodelx->SetValue(Settings::SvCheats::viewmodel::x);
        viewmodely->SetValue(Settings::SvCheats::viewmodel::y);
        viewmodelz->SetValue(Settings::SvCheats::viewmodel::z);
        if(ratio > 0.001)
            aspect->SetValue(ratio);
        else
            aspect->SetValue((35 * 0.1f) / 2);
    }
if (Settings::SvCheats::grenadetraj::enabled)
{
static ConVar* trajectory = cvar->FindVar("sv_grenade_trajectory");
static ConVar* timez = cvar->FindVar("sv_grenade_trajectory_time");
trajectory->SetValue(Settings::SvCheats::grenadetraj::enabled);
timez->SetValue(2);
}
if (Settings::SvCheats::svcheats::enabled)
{
   svcheats->SetValue(Settings::SvCheats::svcheats::enabled);
}

}
