#include "watermark.h"

#include "../ImGUI/imgui.h"
#include "../Utils/draw.h"
#include "../interfaces.h"
#include <time.h>
#include <pwd.h>

std::string GetTime() {
    struct tm tstruct;
    time_t now = time(0);
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);

    return buf;
}

void Watermark::Paint() {
	// std::string text;
	// text += "Kali | ";
	// text += getpwuid(getuid())->pw_name;
	// text += " | ";
	// text += (engine->IsInGame() && csPlayerResource) ? csPlayerResource->GetPing(engine->GetLocalPlayer()) : "0";
	// text += "ms";
	// ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	
	// int w, h,
	// 	x, y;
	// engine->GetScreenSize(w, h);

	// x = w - textSize.x - 5;
	// y = h - textSize.y - 2;

	// ImGui::Text();
}