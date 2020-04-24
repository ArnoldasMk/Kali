#pragma once

#include "../SDK/IInputSystem.h"
#include "../ImGUI/imgui.h"

namespace UI
{
	extern bool isVisible;

	void SetVisible(bool visible);
	void DrawImWatermark();
	void SwapWindow();
	void SetupColors();
	void SetupWindows();
	bool ColorPicker(float* col, bool alphabar);
	bool ColorPicker3(float col[3]);
	bool ColorPicker4(float col[4]);
	void KeyBindButton(ButtonCode_t* key);
	void UpdateWeaponSettings();
	void UpdateRageWeaponSettings();
	void AimBotTargetPicker();
	void ReloadWeaponSettings();
	void ReloadRageWeaponSettings();
}


inline void SetTooltip(const char* text)
{
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", text);
}
