#include "autobuy.h"
#include <string>

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../settings.h"
#include "../fonts.h"
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../Hooks/hooks.h"

void AutoBuy::FireGameEvent(IGameEvent* event)
{
	if (!strstr(event->GetName(), XORSTR("round_start")))
		return;

    if (!Settings::AutoBuy::enabled)
		return;

	std::string buystring;
	switch (Settings::AutoBuy::main)
	{
		case MainWeapon::AUTO:
			buystring += "buy scar20;";
			buystring += "buy g3sg1;"; // TODO: team detection
			break;
		case MainWeapon::AWP:
			buystring += "buy awp;";
			break;

        case MainWeapon::SCOUT:
        	buystring += "buy ssg08;";
			break;
	}

    switch (Settings::AutoBuy::secondary)
    {
    	case SecondaryWeapon::DUAL:
    		buystring += "buy elite;";
    		break;

    	case SecondaryWeapon::DEAGLE:
    		buystring += "buy deagle;";
    		break;
    }

	if (Settings::AutoBuy::nades)
        buystring += "buy molotov;buy incgrenade;buy hegrenade;buy smokegrenade;";
	if (Settings::AutoBuy::armor)
		buystring += "buy vesthelm;";
	if (Settings::AutoBuy::taser)
		buystring += "buy taser;";
	if (Settings::AutoBuy::defuser)
		buystring += "buy defuser;";

	engine->ClientCmd_Unrestricted(buystring.c_str());
}
