#include "EventListener.h"

//#include "Features/bullettracers.h"
#include "Features/legitbot.h"
#include "Features/ragebot.h"
#include "Features/antiaim.h"
#include "Features/eventlog.h"
#include "Features/hitmarkers.h"
#include "Features/hitmarkers3D.h"
#include "Features/namestealer.h"
#include "Features/resolver.h"
#include "Features/skinchanger.h"
#include "Features/spammer.h"
#include "Features/valvedscheck.h"
#include "Features/voterevealer.h"
#include "Features/esp.h"
#include "Features/autobuy.h"
#include "interfaces.h"
#include "SDK/IGameEvent.h"
#include "settings.h"

EventListener::EventListener(std::vector<const char*> events)
{
    for (const auto& it : events)
	gameEvents->AddListener(this, it, false);
}

EventListener::~EventListener()
{
    gameEvents->RemoveListener(this);
}

void EventListener::FireGameEvent(IGameEvent* event)
{
    Legitbot::FireGameEvent(event);
    Ragebot::FireGameEvent(event);
    AntiAim::FireGameEvent(event);
    Hitmarkers::FireGameEvent(event);
    AutoBuy::FireGameEvent(event);
    Eventlog::FireGameEvent(event);
    NameStealer::FireGameEvent(event);
    VoteRevealer::FireGameEvent(event);
    Resolver::FireGameEvent(event);
    Spammer::FireGameEvent(event);
    ValveDSCheck::FireGameEvent(event);
    SkinChanger::FireGameEvent(event);
    ESP::FireGameEvent(event);
}

int EventListener::GetEventDebugID()
{
    return EVENT_DEBUG_ID_INIT;
}
