//#define FGUI_IMPLEMENTATION // make sure this is defined before
// you include the header file
//#include "FGUI/FGUI.hpp"
#include <thread>

#include "hooker.h"
#include "interfaces.h"
#include "Utils/util.h"
#include "fonts.h"
#include "Hooks/hooks.h"
#include "sdlhook.h"
#include "Kali.h"
#include "EventListener.h"
#include "Utils/xorstring.h"
#include "Utils/bonemaps.h"
#include "Features/nosmoke.h"
#include "Features/tracereffect.h"
#include "Features/skinchanger.h"
#include "Features/valvedscheck.h"
#include "settings.h"

static EventListener *eventListener = nullptr;

const char *Util::logFileName = "/tmp/Kali.log";
std::vector<VMT *> createdVMTs;

void MainThread()
{
    Interfaces::FindInterfaces();
    Interfaces::DumpInterfaces();

    cvar->ConsoleDPrintf(XORSTR("Loading...\n"));

    Hooker::FindSetNamedSkybox();
    Hooker::FindViewRender();
    Hooker::FindSDLInput();
    Hooker::FindIClientMode();
    Hooker::FindGlobalVars();
    Hooker::FindCInput();
    Hooker::FindGlowManager();
    Hooker::FindPlayerResource();
    Hooker::FindGameRules();
    Hooker::FindRankReveal();
    Hooker::FindSendClanTag();
    Hooker::FindPrediction();
    Hooker::FindSetLocalPlayerReady();
    Hooker::FindSurfaceDrawing();
    Hooker::FindGetLocalClient();
    Hooker::FindLineGoesThroughSmoke();
    Hooker::FindInitKeyValues();
    Hooker::FindLoadFromBuffer();
    Hooker::FindOverridePostProcessingDisable();
    Hooker::FindPanelArrayOffset();
    Hooker::FindPlayerAnimStateOffset();
    Hooker::FindPlayerAnimOverlayOffset();
    Hooker::FindSequenceActivity();
    Hooker::FindAbsFunctions();
    Hooker::FindItemSystem();
    SDL2::HookSwapWindow();
    SDL2::HookPollEvent();

    Offsets::GetNetVarOffsets();
    Fonts::SetupFonts();
    engineVGuiVMT = new VMT(engineVGui);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking Paint\n"));
    engineVGuiVMT->HookVM(Hooks::Paint, 15);
    engineVGuiVMT->ApplyVMT();
    clientModeVMT = new VMT(clientMode);

    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking OverrideView\n"));
    clientModeVMT->HookVM(Hooks::OverrideView, 19);

    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking CreateMove\n"));
    clientModeVMT->HookVM(Hooks::CreateMove, 25);

    // clientModeVMT->HookVM(Hooks::CreateMove2, 27);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking ShouldDrawCrosshair\n"));
    clientModeVMT->HookVM(Hooks::ShouldDrawCrosshair, 29);

    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking GetViewModelFOV\n"));
    clientModeVMT->HookVM(Hooks::GetViewModelFOV, 36);
    clientModeVMT->ApplyVMT();

    clientVMT = new VMT(client);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking LevelInitPostEntity\n"));
    clientVMT->HookVM(Hooks::LevelInitPostEntity, 6);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking FrameStageNotify\n"));
    clientVMT->HookVM(Hooks::FrameStageNotify, 37);
    clientVMT->ApplyVMT();

    materialVMT = new VMT(material);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking OverrideConfig\n"));
    materialVMT->HookVM(Hooks::OverrideConfig, 21);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking BeginFrame\n"));
    materialVMT->HookVM(Hooks::BeginFrame, 42);
    materialVMT->ApplyVMT();

    gameEventsVMT = new VMT(gameEvents);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking FireEventClientSide\n"));
    gameEventsVMT->HookVM(Hooks::FireEventClientSide, 10);
    gameEventsVMT->ApplyVMT();

    inputInternalVMT = new VMT(inputInternal);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking SetKeyCodeState\n"));
    inputInternalVMT->HookVM(Hooks::SetKeyCodeState, 92);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking SetMouseCodeState\n"));
    inputInternalVMT->HookVM(Hooks::SetMouseCodeState, 93);
    inputInternalVMT->ApplyVMT();

    launcherMgrVMT = new VMT(launcherMgr);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking PumpWindowsMessageLoop\n"));
    launcherMgrVMT->HookVM(Hooks::PumpWindowsMessageLoop, 19);
    launcherMgrVMT->ApplyVMT();

    soundVMT = new VMT(sound);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking EmitSound\n"));
    soundVMT->HookVM(Hooks::EmitSound2, 6);
    soundVMT->ApplyVMT();

    modelRenderVMT = new VMT(modelRender);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking DrawModelExecute\n"));
    modelRenderVMT->HookVM(Hooks::DrawModelExecute, 21);
    modelRenderVMT->ApplyVMT();

    panelVMT = new VMT(panel);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking PaintTraverse\n"));
    panelVMT->HookVM(Hooks::PaintTraverse, 42);
    panelVMT->ApplyVMT();

    viewRenderVMT = new VMT(viewRender);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking RenderView\n"));
    viewRenderVMT->HookVM(Hooks::RenderView, 6);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking RenderSmokePostViewmodel\n"));
    viewRenderVMT->HookVM(Hooks::RenderSmokePostViewmodel, 42);
    viewRenderVMT->ApplyVMT();

    surfaceVMT = new VMT(surface);
    cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 0), XORSTR("Hooking OnScreenSizeChanged\n"));
    surfaceVMT->HookVM(Hooks::OnScreenSizeChanged, 116);
    surfaceVMT->ApplyVMT();

    eventListener = new EventListener({XORSTR("bullet_impact"), XORSTR("cs_game_disconnected"), XORSTR("player_connect_full"), XORSTR("player_death"), XORSTR("item_purchase"), XORSTR("item_remove"), XORSTR("item_pickup"), XORSTR("player_hurt"), XORSTR("bomb_begindefuse"), XORSTR("enter_bombzone"), XORSTR("bomb_beginplant"), XORSTR("switch_team"), XORSTR("round_start"), XORSTR("vote_cast")});

    if (Hooker::HookRecvProp(XORSTR("CBaseViewModel"), XORSTR("m_nSequence"), SkinChanger::sequenceHook))
        SkinChanger::sequenceHook->SetProxyFunction((RecvVarProxyFn)SkinChanger::SetViewModelSequence);

    srand(time(nullptr)); // Seed random # Generator so we can call rand() later

    // Build bonemaps here if we are already in-game
    if (engine->IsInGame())
    {
        BoneMaps::BuildAllBonemaps();
    }
    cvar->ConsoleColorPrintf(ColorRGBA(0, 225, 0), XORSTR("\nKali Successfully loaded.\n"));
}
/* Entrypoint to the Library. Called when loading */
int __attribute__((constructor)) Startup()
{
    std::thread mainThread(MainThread);
    // The root of all suffering is attachment
    // Therefore our little buddy must detach from this realm.
    // Farewell my thread, may we join again some day..
    mainThread.detach();

    return 0;
}
/* Called when un-injecting the library */
void __attribute__((destructor)) Shutdown()
{
    if (Settings::SkyBox::enabled)
    {
        SetNamedSkyBox(cvar->FindVar("sv_skyname")->strValue);
    }
    cvar->FindVar(XORSTR("cl_mouseenable"))->SetValue(1);

    SDL2::UnhookWindow();
    SDL2::UnhookPollEvent();

    NoSmoke::Cleanup();
    TracerEffect::RestoreTracers();

    for (VMT *vmt : createdVMTs)
    {
        delete vmt;
    }

    input->m_fCameraInThirdPerson = false;
    input->m_vecCameraOffset.z = 150.f;
    GetLocalClient(-1)->m_nDeltaTick = -1;

    delete eventListener;

    *s_bOverridePostProcessingDisable = false;

    cvar->ConsoleColorPrintf(ColorRGBA(255, 0, 0), XORSTR("Kali Unloaded successfully.\n"));
}
void Kali::SelfShutdown()
{
    Shutdown();
}
