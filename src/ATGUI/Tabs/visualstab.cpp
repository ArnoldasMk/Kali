#include "visualstab.h"
#include "../SDK/CPlayerResource.h"

#pragma GCC diagnostic ignored "-Wformat-security"

const char *BackendTypes[] = {"Surface (Valve)", "ImGUI (Custom/Faster)"};
const char *BoxTypes[] = {"Flat 2D", "Frame 2D", "Box 3D", "Hitboxes"};
const char *SpriteTypes[] = {"Tux"};
const char *TracerTypes[] = {"Bottom", "Cursor", "Arrows"};
const char *BarTypes[] = {"Vertical Left", "Vertical Right", "Horizontal Below", "Horizontal Above", "Battery"};
const char *BarColorTypes[] = {"Static", "Health Based"};
const char *TeamColorTypes[] = {"Absolute", "Relative"};

const char *chamsTypes[] = {
    "ADDTIVE",
    "ADDTIVE TWO",
    "WIREFRAME",
    "FLAT",
    "PEARL",
    "GLOW",
    "GLOWF",
    "PULSE",
    "ANIMATED",
    "NONE",
};

const char *SmokeTypes[] = {"Wireframe", "None"};
const char *Sounds[] = {"None", "SpongeBob", "Half life", "Half life 2", "Half life 3", "Half life 4", "BB Gun Bell", "Dopamine", "Wub", "Pedo Yes!", "Meme", "Error", "Orchestral", "Gamesense"};
const char *SkyBoxes[] = {
    "cs_baggage_skybox_", // 0
    "cs_tibet",
    "embassy",
    "italy",
    "jungle",
    "office",
    "nukeblank",
    "sky_venice",
    "sky_cs15_daylight01_hdr",
    "sky_cs15_daylight02_hdr",
    "sky_cs15_daylight03_hdr",
    "sky_cs15_daylight04_hdr",
    "sky_csgo_cloudy01",
    "sky_csgo_night_flat",
    "sky_csgo_night02",
    "sky_csgo_night02b",
    "sky_day02_05",
    "sky_day02_05_hdr",
    "sky_dust",
    "vertigo",
    "vertigo_hdr",
    "vertigoblue_hdr",
    "vietnam" // 21
};

const char *tracerEffectNames[] = {
    "Assault Rifle", // 0
    "Pistol",
    "SMG",
    "Rifle",
    "Kisak Snot",
    "Machine Gun",
    "Shotgun",
    "Kisak Snot Fallback",
    "Kisak Snot Fallback2",
    "Wire1A",
    "Wire2",
    "Wire1B",
    "Original",
    "Backup",
    ".50 Cal",
    ".50 Cal Glow",
    ".50 Cal Low",
    ".50 Cal Low Glow", // 17
};

static void FilterEnemies()
{
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Box"), &Settings::ESP::FilterEnemy::Boxes::enabled);
		ImGui::Checkbox(XORSTR("Chams"), &Settings::ESP::FilterEnemy::Chams::enabled);
		ImGui::Checkbox(XORSTR("Health Bar"), &Settings::ESP::FilterEnemy::HelthBar::enabled);
		ImGui::Checkbox(XORSTR("Tracers"), &Settings::ESP::FilterEnemy::Tracers::enabled);
	}
	ImGui::NextColumn();
	{
		ImGui::PushItemWidth(-1);
		ImGui::Combo(XORSTR("##BOXTYPE"), (int *)&Settings::ESP::FilterEnemy::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		ImGui::Combo(XORSTR("##CHAMSTYPE"), (int *)&Settings::ESP::FilterEnemy::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		ImGui::Combo(XORSTR("##BARTYPE"), (int *)&Settings::ESP::FilterEnemy::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		ImGui::Combo(XORSTR("##TRACERTYPE"), (int *)&Settings::ESP::FilterEnemy::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
		ImGui::PopItemWidth();
	}
	ImGui::EndColumns();
	ImGui::Columns(1, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Player Info"), &Settings::ESP::FilterEnemy::playerInfo::enabled);
		ImGui::Checkbox(XORSTR("Head Dot"), &Settings::ESP::FilterEnemy::HeadDot::enabled);
		ImGui::Checkbox(XORSTR("Skeleton"), &Settings::ESP::FilterEnemy::Skeleton::enabled);
		ImGui::Checkbox(XORSTR("Glow"), &Settings::ESP::Glow::enabled);
		ImGui::Checkbox(XORSTR("Dormant"), &Settings::ESP::showDormant);
	}
	// ImGui::SameLine();
	// ColorButton::RenderWindow("Enemy Chams", (int)49, ImGui::ColorButton(XORSTR("Enemy Chams"), (ImVec4)Settings::ESP::enemyVisibleColor.color,0,ImVec2(20,20)));
	// ImGui::SameLine();
	// ColorButton::RenderWindow("Enemy Chams Hidden", (int)50, ImGui::ColorButton(XORSTR("Enemy Chams Hidden"), (ImVec4)Settings::ESP::enemyColor.color,0,ImVec2(20,20)));
	// Under Progress
}

static void FilterLocalPlayer()
{
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Box"), &Settings::ESP::FilterLocalPlayer::Boxes::enabled);
		ImGui::Checkbox(XORSTR("FakeChams"), &Settings::ESP::FilterLocalPlayer::Chams::enabled);
		ImGui::Checkbox(XORSTR("RealChams"), &Settings::ESP::FilterLocalPlayer::RealChams::enabled);
		ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::FilterLocalPlayer::HelthBar::enabled);
		ImGui::Checkbox(XORSTR("Tracers"), &Settings::ESP::FilterLocalPlayer::Tracers::enabled);
	}
	ImGui::NextColumn();
	{
		ImGui::PushItemWidth(-1);
		ImGui::Combo(XORSTR("##BOXTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		ImGui::Combo(XORSTR("##FakeCHAMSTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		ImGui::Combo(XORSTR("##RealCHAMSTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::RealChams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		ImGui::Combo(XORSTR("##BARTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		ImGui::Combo(XORSTR("##TRACERTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
		ImGui::PopItemWidth();
	}
	ImGui::EndColumns();
	ImGui::Columns(1, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Player Info"), &Settings::ESP::FilterLocalPlayer::playerInfo::enabled);
		ImGui::Checkbox(XORSTR("Skeleton"), &Settings::ESP::FilterLocalPlayer::Skeleton::enabled);

		ImGui::Checkbox(XORSTR("Glow"), &Settings::ESP::Glow::enabled);
	}
}

static void FilterAlise()
{
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Outline Box"), &Settings::ESP::FilterAlise::Boxes::enabled);
		ImGui::Checkbox(XORSTR("Chams"), &Settings::ESP::FilterAlise::Chams::enabled);
		ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::FilterAlise::HelthBar::enabled);
		ImGui::Checkbox(XORSTR("Tracers"), &Settings::ESP::FilterAlise::Tracers::enabled);
	}
	ImGui::NextColumn();
	{
		ImGui::PushItemWidth(-1);
		ImGui::Combo(XORSTR("##BOXTYPE"), (int *)&Settings::ESP::FilterAlise::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		ImGui::Combo(XORSTR("##CHAMSTYPE"), (int *)&Settings::ESP::FilterAlise::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		ImGui::Combo(XORSTR("##BARTYPE"), (int *)&Settings::ESP::FilterAlise::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		ImGui::Combo(XORSTR("##TRACERTYPE"), (int *)&Settings::ESP::FilterAlise::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
		ImGui::PopItemWidth();
	}
	ImGui::EndColumns();
	ImGui::Columns(1, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Player Info"), &Settings::ESP::FilterAlise::playerInfo::enabled);
		ImGui::Checkbox(XORSTR("Head Dot"), &Settings::ESP::FilterAlise::HeadDot::enabled);
		ImGui::Checkbox(XORSTR("Skeleton"), &Settings::ESP::FilterAlise::Skeleton::enabled);
		ImGui::Checkbox(XORSTR("Glow"), &Settings::ESP::Glow::enabled);
	}
}

static void colorPicker()
{
	const char *colorNames[IM_ARRAYSIZE(Colors::colors)];
	for (int i = 0; i < IM_ARRAYSIZE(Colors::colors); i++)
		colorNames[i] = Colors::colors[i].name;

	static int colorSelected = 0;

	Settings::UI::Windows::Colors::open = true;

	ImGui::Columns(2, nullptr, false);
	{
		float ButtonsXSize = (ImGui::GetWindowSize().x / 2) - 55;
		ImGui::SetColumnOffset(1, ButtonsXSize);
		ImGui::PushItemWidth(-1);
		ImGui::ListBox(XORSTR("##COLORSELECTION"), &colorSelected, colorNames, IM_ARRAYSIZE(colorNames), 12);
		ImGui::PopItemWidth();
	}
	ImGui::NextColumn();
	{
		if (Colors::colors[colorSelected].type == ColorListVar::HEALTHCOLORVAR_TYPE)
		{
			UI::ColorPicker4((float *)Colors::colors[colorSelected].healthColorVarPtr);
			ImGui::Checkbox(XORSTR("Rainbow"), &Colors::colors[colorSelected].healthColorVarPtr->rainbow);
			ImGui::SameLine();
			ImGui::Checkbox(XORSTR("Health-Based"), &Colors::colors[colorSelected].healthColorVarPtr->hp);
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &Colors::colors[colorSelected].healthColorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
			ImGui::PopItemWidth();
		}
		else
		{
			UI::ColorPicker4((float *)Colors::colors[colorSelected].colorVarPtr);
			ImGui::Checkbox(XORSTR("Rainbow"), &Colors::colors[colorSelected].colorVarPtr->rainbow);
			ImGui::PushItemWidth(-1);
			ImGui::SliderFloat(XORSTR("##RAINBOWSPEED"), &Colors::colors[colorSelected].colorVarPtr->rainbowSpeed, 0.f, 1.f, "Rainbow Speed: %0.3f");
			ImGui::PopItemWidth();
		}
	}
	// Colors::RenderWindow();
}

void Visuals::RenderTab()
{
	float itemWidth = ImGui::GetWindowWidth();

	// Backend For Visuals
	ImGui::Checkbox(XORSTR("Enabled"), &Settings::ESP::enabled);
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	ImGui::Combo(XORSTR("##BACKENDTYPE"), (int *)&Settings::ESP::backend, BackendTypes, IM_ARRAYSIZE(BackendTypes));
	ImGui::PopItemWidth();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Columns(3, nullptr, false);
	{
		ImGui::SetColumnOffset(1, itemWidth / 3);
		ImGui::BeginChild(XORSTR("##FilterEnemyTitle"), ImVec2(0, 22), true);
		ImGui::Text(XORSTR("Enemy"));
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("##FilterLocalPlayerTitle"), ImVec2(0, 22), true);
		ImGui::Text(XORSTR("Local Player"));
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		// ImGui::SetColumnOffset(3, (itemWidth/3)*2 - itemWidth);
		ImGui::BeginChild(XORSTR("##FilterAliseTitle"), ImVec2(0, 22), true);
		ImGui::Text(XORSTR("Alise"));
		ImGui::EndChild();
	}
	ImGui::EndColumns();
	ImGui::Columns(3, nullptr, false);
	{
		ImGui::SetColumnOffset(1, itemWidth / 3);
		ImGui::BeginChild(XORSTR("##FilterEnemy"), ImVec2(0, 150), false);
		FilterEnemies();
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("##FilterLocalPlayer"), ImVec2(0, 150), false);
		FilterLocalPlayer();
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		// ImGui::SetColumnOffset(3, (itemWidth/3)*2 - itemWidth);
		ImGui::BeginChild(XORSTR("##FilterAlise"), ImVec2(0, 150), false);
		FilterAlise();
		ImGui::EndChild();
	}
	ImGui::EndColumns();

	// Filter Visibility
	ImGui::Columns(2, nullptr, false);
	{
		// if( Settings::ESP::backend == DrawingBackend::IMGUI ){
		//  ImGui::Checkbox( XORSTR( "Aliased Lines"), &Settings::UI::imGuiAliasedLines );
		// 	ImGui::SameLine();
		//     ImGui::Checkbox( XORSTR( "Aliased Fill"), &Settings::UI::imGuiAliasedFill );
		// }
		//END Backend for visualas options
		// ImGui::PushItemWidth(-1);
		// ImGui::Combo(XORSTR("##Filters"), (int*)&Settings::ESP::filter, Filter, IM_ARRAYSIZE(Filter));
		// ImGui::PopItemWidth();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::BeginChild(XORSTR("##Visuals"), ImVec2(0, 0), false);
		{
			ImGui::Columns(1, nullptr, false);
			ImGui::PushItemWidth(-1);
			if (ImGui::BeginCombo(XORSTR("##Filter Visibility"), XORSTR("Visibility Filter")))
			{
				ImGui::Selectable(XORSTR("Smoke Check"), &Settings::ESP::Filters::smokeCheck, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Legit Mode"), &Settings::ESP::Filters::legit, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable(XORSTR("Visibility Check"), &Settings::ESP::Filters::visibilityCheck, ImGuiSelectableFlags_DontClosePopups);
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo(XORSTR("##FilterOptions"), XORSTR("Filter Options")))
			{

				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Selectable(XORSTR("Clan"), &Settings::ESP::Info::clan, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Rank"), &Settings::ESP::Info::rank, ImGuiSelectableFlags_DontClosePopups);
					ImGui::PushID(1);
					ImGui::Selectable(XORSTR("Health"), &Settings::ESP::Info::health, ImGuiSelectableFlags_DontClosePopups);
					ImGui::PopID();
					ImGui::Selectable(XORSTR("Armor"), &Settings::ESP::Info::armor, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Scoped"), &Settings::ESP::Info::scoped, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Flashed"), &Settings::ESP::Info::flashed, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Defuse Kit"), &Settings::ESP::Info::hasDefuser, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Grabbing Hostage"), &Settings::ESP::Info::grabbingHostage, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Location"), &Settings::ESP::Info::location, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("FakeDuck"), &Settings::ESP::Info::Fakeduck, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::NextColumn();
				{
					ImGui::Selectable(XORSTR("Name"), &Settings::ESP::Info::name, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Steam ID"), &Settings::ESP::Info::steamId, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Weapon"), &Settings::ESP::Info::weapon, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Reloading"), &Settings::ESP::Info::reloading, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Planting"), &Settings::ESP::Info::planting, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Defusing"), &Settings::ESP::Info::defusing, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Resolver Flags"), &Settings::ESP::Info::rescuing, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Layers Debug"), &Settings::Debug::AnimLayers::draw, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Choked Packets"), &Settings::ESP::Info::money, ImGuiSelectableFlags_DontClosePopups);
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo(XORSTR("##WorldItems"), XORSTR("World Items")))
			{
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Selectable(XORSTR("Weapons"), &Settings::ESP::Filters::weapons, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Throwables"), &Settings::ESP::Filters::throwables, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::NextColumn();
				{
					ImGui::Selectable(XORSTR("Bomb"), &Settings::ESP::Filters::bomb, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Defuse Kits"), &Settings::ESP::Filters::defusers, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Hostages"), &Settings::ESP::Filters::hostages, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo(XORSTR("##DangerZone"), XORSTR("Danger Zone")))
			{
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Selectable(XORSTR("Loot Crates"), &Settings::ESP::DangerZone::lootcrate, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Weapon Upgrades"), &Settings::ESP::DangerZone::upgrade, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Ammo box"), &Settings::ESP::DangerZone::ammobox, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Radar Jammer"), &Settings::ESP::DangerZone::radarjammer, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Cash"), &Settings::ESP::DangerZone::cash, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Drone"), &Settings::ESP::DangerZone::drone, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Draw Distance"), &Settings::ESP::DangerZone::drawDistEnabled, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::NextColumn();
				{
					ImGui::Selectable(XORSTR("Safe"), &Settings::ESP::DangerZone::safe, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Sentry Turret"), &Settings::ESP::DangerZone::dronegun, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Melee"), &Settings::ESP::DangerZone::melee, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Tablet"), &Settings::ESP::DangerZone::tablet, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Healthshot"), &Settings::ESP::DangerZone::healthshot, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Explosive Barrel"), &Settings::ESP::DangerZone::barrel, ImGuiSelectableFlags_DontClosePopups);
					if (Settings::ESP::DangerZone::drawDistEnabled)
						ImGui::SliderInt(XORSTR("##DZDRAWDIST"), &Settings::ESP::DangerZone::drawDist, 1, 10000, XORSTR("Amount: %0.f"));
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo(XORSTR("##CrossHairOptions"), XORSTR("Crosshair Options")))
			{
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Selectable(XORSTR("Recoil Crosshair"), &Settings::Recoilcrosshair::enabled, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("FOV Circle"), &Settings::ESP::FOVCrosshair::enabled, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Show Spread"), &Settings::ESP::Spread::enabled, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Velocity Graph"), &Settings::ESP::VelGraph, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::NextColumn();
				{
					ImGui::Selectable(XORSTR("Only When Shooting"), &Settings::Recoilcrosshair::showOnlyWhenShooting, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Filled"), &Settings::ESP::FOVCrosshair::filled, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Show SpreadLimit"), &Settings::ESP::Spread::spreadLimit, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Show Indicators"), &Settings::ESP::indicators::enabled, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			colorPicker(); // Converting Color Picker window in this place
			/*
			ImGui::Separator();
			ImGui::Text(XORSTR("Event logger"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::Checkbox(XORSTR("Show Enemies"), &Settings::Eventlog::showEnemies);
				ImGui::Checkbox(XORSTR("Show Allies"), &Settings::Eventlog::showTeammates);

			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##LOGGERDURATION"), &Settings::Eventlog::duration, 1000.f, 5000.f, XORSTR("Log duration: %0.f"));
				ImGui::SliderFloat(XORSTR("##LOGGERLINES"), &Settings::Eventlog::lines, 5, 15, XORSTR("Log lines: %0.f"));
				ImGui::Checkbox(XORSTR("Show LocalPlayer"), &Settings::Eventlog::showLocalplayer);
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);
			*/
			ImGui::EndChild();
		}
	}

	ImGui::NextColumn();
	{
		ImGui::Text(XORSTR("Only on Key"));
		UI::KeyBindButton(&Settings::ESP::key);
		ImGui::BeginChild(XORSTR("Chams"), ImVec2(0, 0), false);
		{
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::Checkbox(XORSTR("Skybox Changer"), &Settings::SkyBox::enabled);
				ImGui::Checkbox(XORSTR("Sleeves"), &Settings::ESP::Chams::Sleeves::enabled);
				ImGui::Checkbox(XORSTR("Arms"), &Settings::ESP::Chams::Arms::enabled);
				ImGui::Checkbox(XORSTR("Weapons"), &Settings::ESP::Chams::Weapon::enabled);
				ImGui::Checkbox(XORSTR("Dlights"), &Settings::Dlights::enabled);
				ImGui::Checkbox(XORSTR("No Flash"), &Settings::Noflash::enabled);
				ImGui::Checkbox(XORSTR("No Smoke"), &Settings::NoSmoke::enabled);
				ImGui::Checkbox(XORSTR("DrawAATrace"), &Settings::ESP::DrawAATrace::enabled);
				ImGui::Checkbox(XORSTR("Show Footsteps"), &Settings::ESP::Sounds::enabled);
				ImGui::Checkbox(XORSTR("No View Punch"), &Settings::View::NoViewPunch::enabled);
				ImGui::Checkbox(XORSTR("No Sky"), &Settings::NoSky::enabled);
				ImGui::Checkbox(XORSTR("Show Keybinds"), &Settings::ESP::KeyBinds);
				if (Settings::ESP::KeyBinds)
				{
					int width, height;
					ImGui::SliderInt(XORSTR("##X"), &Settings::ESP::keybi::x, 0, 1920, XORSTR("X: %0.f"));
					ImGui::SliderInt(XORSTR("##Y"), &Settings::ESP::keybi::y, 0, 1080, XORSTR("Y: %0.f"));
				}
				ImGui::Checkbox(XORSTR("Watermark"), &Settings::ESP::Watermark::enabled);
				ImGui::Checkbox(XORSTR("Show Hitbox impacts"), &Settings::ESP::showimpacts);
				ImGui::Checkbox(XORSTR("Draw Bullet Tracers"), &Settings::ESP::tracebullet::enabled);
				if (Settings::ESP::tracebullet::enabled)
				{
					ImGui::Checkbox(XORSTR("Localplayer only"), &Settings::ESP::tracebullet::local);
				}
				ImGui::Checkbox(XORSTR("Sync fake chams with fakelag"), &Settings::ESP::SyncFake);
				ImGui::Checkbox(XORSTR("Draw Backtrack Chams"), &Settings::Ragebot::backTrack::draw);

				if (ImGui::Button(XORSTR("Material Config"), ImVec2(-1, 0)))
					ImGui::OpenPopup(XORSTR("##MaterialConfigWindow"));
				SetTooltip(XORSTR("Advanced CSGO Gfx Settings\nExperimental"));
				ImGui::SetNextWindowSize(ImVec2(320, 640), ImGuiCond_Always);
				if (ImGui::BeginPopup(XORSTR("##MaterialConfigWindow")))
				{
					ImGui::PushItemWidth(-1);
					if (ImGui::Button(XORSTR("Reset Changes")))
					{
						Settings::MaterialConfig::config = MaterialConfig::backupConfig;
					}
					ImGui::Checkbox(XORSTR("Changes Enabled?"), &Settings::MaterialConfig::enabled);
					SetTooltip(XORSTR("Expect some lag when changing these settings.\nIf your hud breaks, toggle cl_drawhud off/on"));
					ImGui::SliderFloat(XORSTR("##MONITORGAMMA"), &Settings::MaterialConfig::config.m_fMonitorGamma, 0.1f, 12.0f,
								    XORSTR("Gamma: %.3f"));
					ImGui::SliderFloat(XORSTR("##GAMMATVRANGEMIN"), &Settings::MaterialConfig::config.m_fGammaTVRangeMin,
								    0.1f, std::min(300.0f, Settings::MaterialConfig::config.m_fGammaTVRangeMax),
								    XORSTR("TVRangeMin: %.3f"));
					ImGui::SliderFloat(XORSTR("##GAMMATVRANGEMAX"), &Settings::MaterialConfig::config.m_fGammaTVRangeMax,
								    Settings::MaterialConfig::config.m_fGammaTVRangeMin, 300.0f,
								    XORSTR("TVRangeMax: %.3f"));
					ImGui::SliderFloat(XORSTR("##GAMMATVEXPONENT"), &Settings::MaterialConfig::config.m_fGammaTVExponent,
								    0.1f, 3.0f, XORSTR("TV Exponent: %.3f"));
					ImGui::Checkbox(XORSTR("GammaTVEnabled"), &Settings::MaterialConfig::config.m_bGammaTVEnabled);
					ImGui::Text(XORSTR("Width:"));
					ImGui::SameLine();
					ImGui::InputInt(XORSTR("##GAMEWIDTH"), &Settings::MaterialConfig::config.m_VideoMode.m_Width);

					ImGui::Text(XORSTR("Height:"));
					ImGui::SameLine();
					ImGui::InputInt(XORSTR("##GAMEHEIGHT"), &Settings::MaterialConfig::config.m_VideoMode.m_Height);

					ImGui::Text(XORSTR("Refresh Rate:"));
					ImGui::SameLine();
					ImGui::InputInt(XORSTR("##GAMEREFRESHRATE"),
								 &Settings::MaterialConfig::config.m_VideoMode.m_RefreshRate);

					ImGui::Checkbox(XORSTR("TripleBuffered"), &Settings::MaterialConfig::config.m_bTripleBuffered);
					ImGui::SliderInt(XORSTR("##AASAMPLES"), &Settings::MaterialConfig::config.m_nAASamples, 0, 16, XORSTR("AA Samples: %1.f"));
					ImGui::SliderInt(XORSTR("##FORCEANISOTROPICLEVEL"), &Settings::MaterialConfig::config.m_nForceAnisotropicLevel, 0, 8, XORSTR("Anisotropic Level: %1.f"));
					ImGui::SliderInt(XORSTR("##SKIPMIPLEVELS"), &Settings::MaterialConfig::config.skipMipLevels, 0, 10, XORSTR("SkipMipLevels: %1.f"));
					SetTooltip(XORSTR("Makes the game flatter."));
					if (ImGui::Button(XORSTR("Flags"), ImVec2(-1, 0)))
						ImGui::OpenPopup(XORSTR("##MaterialConfigFlags"));
					ImGui::SetNextWindowSize(ImVec2(320, 240), ImGuiCond_Always);
					if (ImGui::BeginPopup(XORSTR("##MaterialConfigFlags")))
					{
						ImGui::PushItemWidth(-1);

						static bool localFlags[] = {
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_WINDOWED) != 0,		    // ( 1 << 0 )
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_RESIZING) != 0,		    // ( 1 << 1 )
						    false,																						    // ( 1 << 2 ) is not used.
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_NO_WAIT_FOR_VSYNC) != 0, // ( 1 << 3 )
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_STENCIL) != 0,		    // ...
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_TRILINEAR) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_HWSYNC) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_SPECULAR) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_BUMPMAP) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_PARALLAX_MAPPING) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USE_Z_PREFILL) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_REDUCE_FILLRATE) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_HDR) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_LIMIT_WINDOWED_SIZE) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_SCALE_TO_OUTPUT_RESOLUTION) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USING_MULTIPLE_WINDOWS) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_PHONG) != 0,
						    (Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_VR_MODE) != 0};

						ImGui::Checkbox(XORSTR("Windowed"), &localFlags[0]);
						ImGui::Checkbox(XORSTR("Resizing"), &localFlags[1]);
						ImGui::Checkbox(XORSTR("No VSYNC Wait"), &localFlags[3]);
						ImGui::Checkbox(XORSTR("Stencil"), &localFlags[4]);
						ImGui::Checkbox(XORSTR("Force Tri-Linear"), &localFlags[5]);
						ImGui::Checkbox(XORSTR("Force HW Sync"), &localFlags[6]);
						ImGui::Checkbox(XORSTR("Disable Specular"), &localFlags[7]);
						ImGui::Checkbox(XORSTR("Disable Bumpmap"), &localFlags[8]);
						ImGui::Checkbox(XORSTR("Disable Phong"), &localFlags[16]);
						ImGui::Checkbox(XORSTR("Parallax Mapping"), &localFlags[9]);
						ImGui::Checkbox(XORSTR("Use Z-Prefill"), &localFlags[10]);
						ImGui::Checkbox(XORSTR("Reduce FillRate"), &localFlags[11]);
						ImGui::Checkbox(XORSTR("HDR"), &localFlags[12]);
						ImGui::Checkbox(XORSTR("Limit Windowed Size"), &localFlags[13]);
						ImGui::Checkbox(XORSTR("Scale to Output Resolution"), &localFlags[14]);
						ImGui::Checkbox(XORSTR("Using Multiple Windows"), &localFlags[15]);
						ImGui::Checkbox(XORSTR("VR-Mode"), &localFlags[17]);

						if (ImGui::Button(XORSTR("Apply ")))
						{
							for (unsigned short i = 0; i < 18; i++)
							{
								if (i == 2) // ( 1 << 2 ) not used.
									continue;
								Settings::MaterialConfig::config.SetFlag((unsigned int)(1 << i), localFlags[i]);
							}
						}

						ImGui::PopItemWidth();
						ImGui::EndPopup();
					}
					//m_flags
					ImGui::Checkbox(XORSTR("EditMode"), &Settings::MaterialConfig::config.bEditMode);
					//proxiesTestMode
					ImGui::Checkbox(XORSTR("Compressed Textures"),
								 &Settings::MaterialConfig::config.bCompressedTextures);
					ImGui::Checkbox(XORSTR("Filter Lightmaps"), &Settings::MaterialConfig::config.bFilterLightmaps);
					ImGui::Checkbox(XORSTR("Filter Textures"), &Settings::MaterialConfig::config.bFilterTextures);
					ImGui::Checkbox(XORSTR("Reverse Depth"), &Settings::MaterialConfig::config.bReverseDepth);
					ImGui::Checkbox(XORSTR("Buffer Primitives"), &Settings::MaterialConfig::config.bBufferPrimitives);
					ImGui::Checkbox(XORSTR("Draw Flat"), &Settings::MaterialConfig::config.bDrawFlat);
					ImGui::Checkbox(XORSTR("Measure Fill-Rate"), &Settings::MaterialConfig::config.bMeasureFillRate);
					ImGui::Checkbox(XORSTR("Visualize Fill-Rate"),
								 &Settings::MaterialConfig::config.bVisualizeFillRate);
					ImGui::Checkbox(XORSTR("No Transparency"), &Settings::MaterialConfig::config.bNoTransparency);
					ImGui::Checkbox(XORSTR("Software Lighting"),
								 &Settings::MaterialConfig::config.bSoftwareLighting); // Crashes game
					//AllowCheats ?
					ImGui::SliderInt(XORSTR("##MIPLEVELS"), (int *)&Settings::MaterialConfig::config.nShowMipLevels, 0,
								  3, XORSTR("ShowMipLevels: %1.f"));
					ImGui::Checkbox(XORSTR("Show Low-Res Image"), &Settings::MaterialConfig::config.bShowLowResImage);
					ImGui::Checkbox(XORSTR("Show Normal Map"), &Settings::MaterialConfig::config.bShowNormalMap);
					ImGui::Checkbox(XORSTR("MipMap Textures"), &Settings::MaterialConfig::config.bMipMapTextures);
					ImGui::SliderInt(XORSTR("##NFULLBRIGHT"), (int *)&Settings::MaterialConfig::config.nFullbright, 0, 3, XORSTR("nFullBright: %1.f"));
					SetTooltip(XORSTR("1 = Bright World, 2 = Bright Models"));
					ImGui::Checkbox(XORSTR("Fast NoBump"), &Settings::MaterialConfig::config.m_bFastNoBump);
					ImGui::Checkbox(XORSTR("Suppress Rendering"),
								 &Settings::MaterialConfig::config.m_bSuppressRendering);
					ImGui::Checkbox(XORSTR("Draw Gray"), &Settings::MaterialConfig::config.m_bDrawGray);
					ImGui::Checkbox(XORSTR("Show Specular"), &Settings::MaterialConfig::config.bShowSpecular);
					ImGui::Checkbox(XORSTR("Show Defuse"), &Settings::MaterialConfig::config.bShowDiffuse);
					ImGui::SliderInt(XORSTR("##AAQUALITY"), &Settings::MaterialConfig::config.m_nAAQuality, 0, 16,
								  XORSTR("AAQuality: %1.f"));
					ImGui::Checkbox(XORSTR("Shadow Depth Texture"),
								 &Settings::MaterialConfig::config.m_bShadowDepthTexture);
					SetTooltip(XORSTR("Risky. May cause black Screen. Reset if it does."));
					ImGui::Checkbox(XORSTR("Motion Blur"), &Settings::MaterialConfig::config.m_bMotionBlur);
					ImGui::Checkbox(XORSTR("Support Flashlight"),
								 &Settings::MaterialConfig::config.m_bSupportFlashlight);
					ImGui::Checkbox(XORSTR("Paint Enabled"), &Settings::MaterialConfig::config.m_bPaintEnabled);
					// VRMode Adapter?
					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
				if (ImGui::Button(XORSTR("Tracer Effect"), ImVec2(-1, 0)))
					ImGui::OpenPopup(XORSTR("##TracerEffectWindow"));
				ImGui::SetNextWindowSize(ImVec2(320, 120), ImGuiCond_Always);
				if (ImGui::BeginPopup(XORSTR("##TracerEffectWindow")))
				{
					ImGui::PushItemWidth(-1);
					if (Settings::TracerEffects::serverSide)
					{
						Settings::TracerEffects::frequency = 1;
						Settings::TracerEffects::effect = TracerEffects_t::TASER;
					}
					ImGui::Combo(XORSTR("##TracerEffects"), (int *)&Settings::TracerEffects::effect, tracerEffectNames, IM_ARRAYSIZE(tracerEffectNames));
					ImGui::Checkbox(XORSTR("Enable Tracers"), &Settings::TracerEffects::enabled);
					ImGui::Checkbox(XORSTR("Server Sided?"), &Settings::TracerEffects::serverSide);
					SetTooltip(XORSTR("Requires a Taser in your Inventory.\nCan only shoot one shot at a time\nOnly Works with Kisak Snot"));
					ImGui::Columns(2, nullptr, false);
					{
						ImGui::SliderInt(XORSTR("##TracerFreq"), &Settings::TracerEffects::frequency, 0, 10, XORSTR("Freq: %0.f"));
					}
					ImGui::NextColumn();
					{
						if (ImGui::Button(XORSTR("Restore Tracers")))
						{
							TracerEffect::RestoreTracers();
						}
					}
					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
			}
			ImGui::NextColumn();
			{
				ImGui::Combo(XORSTR("##SKYBOX"), &Settings::SkyBox::skyBoxNumber, SkyBoxes, IM_ARRAYSIZE(SkyBoxes));
				ImGui::PushItemWidth(-1);
				ImGui::Combo(XORSTR("##SLEEVESTYPE"), (int *)&Settings::ESP::Chams::Sleeves::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				ImGui::Combo(XORSTR("##ARMSTYPE"), (int *)&Settings::ESP::Chams::Arms::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				ImGui::Combo(XORSTR("##WEAPONTYPE"), (int *)&Settings::ESP::Chams::Weapon::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				ImGui::SliderFloat(XORSTR("##DLIGHTRADIUS"), &Settings::Dlights::radius, 0, 1000, XORSTR("Radius: %0.f"));
				ImGui::SliderFloat(XORSTR("##NOFLASHAMOUNT"), &Settings::Noflash::value, 0, 255, XORSTR("Amount: %0.f"));
				ImGui::Combo(XORSTR("##SMOKETYPE"), (int *)&Settings::NoSmoke::type, SmokeTypes, IM_ARRAYSIZE(SmokeTypes));
				ImGui::SliderInt(XORSTR("##SOUNDSTIME"), &Settings::ESP::Sounds::time, 250, 5000, XORSTR("Timeout: %0.f"));
				ImGui::PopItemWidth();
				ImGui::Checkbox(XORSTR("Grenade Prediction"), &Settings::GrenadePrediction::enabled);
				ImGui::Checkbox(XORSTR("Draw Molotov"), &Settings::ESP::Drawfire::enabled);
				ImGui::Checkbox(XORSTR("No Aim Punch"), &Settings::View::NoAimPunch::enabled);
				ImGui::Checkbox(XORSTR("ASUS Walls"), &Settings::ASUSWalls::enabled);
				ImGui::Checkbox(XORSTR("Draw Taser Range"), &Settings::ESP::taserrange::enabled);
				ImGui::Checkbox(XORSTR("No Scope Border"), &Settings::NoScopeBorder::enabled);
				ImGui::Checkbox(XORSTR("Autowall Debug"), &Settings::Debug::AutoWall::debugView);
				ImGui::Checkbox(XORSTR("AimSpot Debug"), &Settings::Debug::AutoAim::drawTarget);
				ImGui::Checkbox(XORSTR("BoneMap Debug"), &Settings::Debug::BoneMap::draw);
				if (Settings::Debug::BoneMap::draw)
					ImGui::Checkbox(XORSTR("Just Dots"), &Settings::Debug::BoneMap::justDrawDots);
				ImGui::SliderInt(XORSTR("Test Model ID"), &Settings::Debug::BoneMap::modelID, 1253, 1350, XORSTR("Model ID: %0.f"));
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Radar"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::Checkbox(XORSTR("Radar"), &Settings::Radar::enabled);
				ImGui::PushItemWidth(-1);
				ImGui::Combo(XORSTR("##RADARTEAMCOLTYPE"), (int *)&Settings::Radar::teamColorType, TeamColorTypes, IM_ARRAYSIZE(TeamColorTypes));
				ImGui::PopItemWidth();
				ImGui::Checkbox(XORSTR("Enemies"), &Settings::Radar::enemies);
				ImGui::Checkbox(XORSTR("Bomb"), &Settings::Radar::bomb);
				ImGui::Checkbox(XORSTR("Legit"), &Settings::Radar::legit);
				ImGui::Checkbox(XORSTR("In-game Radar"), &Settings::Radar::InGame::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##RADARZOOM"), &Settings::Radar::zoom, 0.f, 100.f, XORSTR("Zoom: %0.f"));
				ImGui::SliderFloat(XORSTR("##RADARICONSSCALE"), &Settings::Radar::iconsScale, 2, 16, XORSTR("Icons Scale: %0.1f"));
				ImGui::PopItemWidth();
				ImGui::Checkbox(XORSTR("Allies"), &Settings::Radar::allies);
				ImGui::Checkbox(XORSTR("Defuser"), &Settings::Radar::defuser);
				ImGui::Checkbox(XORSTR("Visibility Check"), &Settings::Radar::visibilityCheck);
				ImGui::Checkbox(XORSTR("Smoke Check"), &Settings::Radar::smokeCheck);
			}
			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::Text(XORSTR("Hitmarkers"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::Checkbox(XORSTR("Hitmarkers"), &Settings::ESP::Hitmarker::enabled);
				if (Settings::ESP::Hitmarker::enabled)
				{
					ImGui::Checkbox(XORSTR("Enemies##HITMARKERS"), &Settings::ESP::Hitmarker::enemies);
					ImGui::Checkbox(XORSTR("Allies##HITMARKERS"), &Settings::ESP::Hitmarker::allies);
					ImGui::Checkbox(XORSTR("Sounds##HITMARKERS"), &Settings::ESP::Hitmarker::Sounds::enabled);
					ImGui::Checkbox(XORSTR("Damage##HITMARKERS"), &Settings::ESP::Hitmarker::Damage::enabled);
				}
			}
			ImGui::NextColumn();
			{
				if (Settings::ESP::Hitmarker::enabled)
				{

					ImGui::PushItemWidth(-1);
					ImGui::SliderInt(XORSTR("##HITMARKERDUR"), &Settings::ESP::Hitmarker::duration, 250, 3000, XORSTR("Timeout: %0.f"));
					ImGui::SliderInt(XORSTR("##HITMARKERSIZE"), &Settings::ESP::Hitmarker::size, 1, 32, XORSTR("Size: %0.f"));
					ImGui::SliderInt(XORSTR("##HITMARKERGAP"), &Settings::ESP::Hitmarker::innerGap, 1, 16, XORSTR("Gap: %0.f"));
					ImGui::Combo(XORSTR("Sounds##HITMARKERCOMBO"), (int *)&Settings::ESP::Hitmarker::Sounds::sound, Sounds, IM_ARRAYSIZE(Sounds));
					ImGui::PopItemWidth();
				}
			}
			ImGui::Separator();

			ImGui::Text(XORSTR("Fog modulation"));
			ImGui::Separator();
			ImGui::Columns(1);
			{
				ImGui::Checkbox(XORSTR("Enabled"), &Settings::ESP::customfog::enabled);
				if (Settings::ESP::customfog::enabled)
				{
					ImGui::SliderFloat(XORSTR("##CUSTOMFOGDENSITY"), &Settings::ESP::customfog::density, 0, 100, XORSTR("Density: %0.f"));
					ImGui::SliderInt(XORSTR("##CUSTOMFOGDISTANCE"), &Settings::ESP::customfog::distance, 0, 2500, XORSTR("Distance: %0.f"));
				}
			}

			ImGui::Separator();
			ImGui::Text(XORSTR("Event logger"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::Checkbox(XORSTR("Show Enemies"), &Settings::Eventlog::showEnemies);
				ImGui::Checkbox(XORSTR("Show Allies"), &Settings::Eventlog::showTeammates);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##LOGGERDURATION"), &Settings::Eventlog::duration, 1000.f, 5000.f, XORSTR("Log duration: %0.f"));
				ImGui::SliderFloat(XORSTR("##LOGGERLINES"), &Settings::Eventlog::lines, 5, 15, XORSTR("Log lines: %0.f"));
				ImGui::Checkbox(XORSTR("Show LocalPlayer"), &Settings::Eventlog::showLocalplayer);
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("OOV Arrow Settings"));
			ImGui::Separator();
			ImGui::SliderInt(XORSTR("##ARROWDISTANCE"), &Settings::ESP::arrows::distance, 1, 100, XORSTR("Arrows distance: %0.f"));
			ImGui::SliderInt(XORSTR("##ARROWSIZE"), &Settings::ESP::arrows::size, 1, 100, XORSTR("Arrows size: %0.f"));

			ImGui::EndChild();
		}
	}
}
