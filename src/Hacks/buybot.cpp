#include "buybot.h"
#include <string>

#include "../Utils/xorstring.h"
#include "../Utils/entity.h"
#include "../settings.h"
#include "../fonts.h"
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../Hooks/hooks.h"

void buybot::FireGameEvent(IGameEvent* event)
{

	if (strstr(event->GetName(), XORSTR("round_start"))){
        if (!Settings::buybot::enabled)
		return;
	std::string buystring;
	switch (Settings::buybot::wep)
	{
		case MainWep::AUTO:
		buystring += "buy scar20;";
		buystring += "buy g3sg1;";
		break;

		case MainWep::AWP:
		buystring += "buy awp;";
                break;

                case MainWep::SCOUT:
                buystring += "buy ssg08;";
		break;
	}

        switch (Settings::buybot::sec)
        {
        	case SecWep::DUAL:
        	buystring += "buy elite;";
        	break;

        	case SecWep::DEAGLE:
        	buystring += "buy deagle;";
        	break;
        }

	if (Settings::buybot::scout)
	                buystring += "buy molotov;buy incgrenade;buy hegrenade;buy smokegrenade;buy defuser;";
	if (Settings::buybot::autosniper)
		buystring += "buy vesthelm;";
	if (Settings::buybot::awp)
		buystring += "buy taser;";

	engine->ClientCmd_Unrestricted(buystring.c_str());
    }
}
