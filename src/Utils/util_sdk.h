#pragma once

#include <string>
#include "../SDK/IInputSystem.h"
#include "../SDK/IMaterial.h"

namespace Util
{
	char GetButtonString(ButtonCode_t key);
	std::string GetButtonName(ButtonCode_t buttonCode);
	ButtonCode_t GetButtonCode(std::string buttonName);

	IMaterial* CreateMaterial(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert );
	IMaterial* CreateMaterial( bool ignorez, std::string envmap, std::string envmaptint, int envmapfresnel, std::string envmapfresnelminmaxexp);
	IMaterial* CreateMaterial( std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert, std::string envmap, std::string envmaptint, int envmapfresnel, std::string envmapfresnelminmaxexp);
	IMaterial* CreateMaterial2(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, float alpha);
        IMaterial* CreateMaterial3(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, float alpha);

	const char *GetActivityName(int actNum);

	bool IsDangerZone(); // TODO: Use interfaces since we are internal.
}
