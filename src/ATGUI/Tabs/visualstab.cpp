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

static void colorPicker()
{
	const char *colorNames[IM_ARRAYSIZE(Colors::colors)];
	for (int i = 0; i < IM_ARRAYSIZE(Colors::colors); i++)
		colorNames[i] = Colors::colors[i].name;

	static int colorSelected = 0;

	Settings::UI::Windows::Colors::open = true;

	ImGui::Columns(2, nullptr, false);
	{
		float ButtonsXSize = (ImGui::GetWindowSize().x / 2) + 55;
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

void VisualsMaterialConfig::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
	{
		ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::ESP::enabled);
	}
	ImGui::EndGroup();

	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
		ImGui::Columns(3, nullptr, false);
		{
			ImGui::SetColumnOffset(1, 500);
			ImGui::BeginChild(XORSTR("##PlayerVisuals1"), ImVec2(0, 736), true);
			{
				// ImGui::Separator();
				// ImGui::Columns(1);
				ImGui::Text(XORSTR("Display Configuration"));
				ImGui::Separator();

				ImGui::Text(XORSTR("Width:"));
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);

				ImGui::InputInt(XORSTR("##GAMEWIDTH"), &Settings::MaterialConfig::config.m_VideoMode.m_Width);

				ImGui::Text(XORSTR("Height:"));
				ImGui::SameLine();
				ImGui::InputInt(XORSTR("##GAMEHEIGHT"), &Settings::MaterialConfig::config.m_VideoMode.m_Height);

				ImGui::Text(XORSTR("RefreshRate:"));
				ImGui::SameLine();
				ImGui::InputInt(XORSTR("##GAMEREFRESHRATE"),
							 &Settings::MaterialConfig::config.m_VideoMode.m_RefreshRate);
				ImGui::PopItemWidth();

				ImGui::Separator();
				ImGui::Columns(1);
				ImGui::Text(XORSTR("Material Config"));
				ImGui::Separator();
				static bool localFlags[] = {
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_WINDOWED) != 0, // ( 1 << 0 )
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_RESIZING) != 0, // ( 1 << 1 )
				    false,												  // ( 1 << 2 ) is not used.
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_NO_WAIT_FOR_VSYNC) != 0, // ( 1 << 3 )
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_STENCIL) != 0, // ...
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_TRILINEAR) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_HWSYNC) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_SPECULAR) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_BUMPMAP) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_PARALLAX_MAPPING) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USE_Z_PREFILL) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_REDUCE_FILLRATE) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_HDR) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_LIMIT_WINDOWED_SIZE) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_SCALE_TO_OUTPUT_RESOLUTION) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USING_MULTIPLE_WINDOWS) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_PHONG) != 0,
				    (Settings::MaterialConfig::config.m_Flags &
					MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_VR_MODE) != 0,
				};

				ImGui::CheckboxFill(XORSTR("Windowed"), &localFlags[0]);
				ImGui::CheckboxFill(XORSTR("Resizing"), &localFlags[1]);
				ImGui::CheckboxFill(XORSTR("No VSYNC Wait"), &localFlags[3]);
				ImGui::CheckboxFill(XORSTR("Stencil"), &localFlags[4]);
				ImGui::CheckboxFill(XORSTR("Force Tri-Linear"), &localFlags[5]);
				ImGui::CheckboxFill(XORSTR("Force HW Sync"), &localFlags[6]);
				ImGui::CheckboxFill(XORSTR("Disable Specular"), &localFlags[7]);
				ImGui::CheckboxFill(XORSTR("Disable Bumpmap"), &localFlags[8]);
				ImGui::CheckboxFill(XORSTR("Disable Phong"), &localFlags[16]);
				ImGui::CheckboxFill(XORSTR("Parallax Mapping"), &localFlags[9]);
				ImGui::CheckboxFill(XORSTR("Use Z-Prefill"), &localFlags[10]);
				ImGui::CheckboxFill(XORSTR("Reduce FillRate"), &localFlags[11]);
				ImGui::CheckboxFill(XORSTR("HDR"), &localFlags[12]);
				ImGui::CheckboxFill(XORSTR("Limit Windowed Size"), &localFlags[13]);
				ImGui::CheckboxFill(XORSTR("Scale to Output Resolution"), &localFlags[14]);
				ImGui::CheckboxFill(XORSTR("Using Multiple Windows"), &localFlags[15]);
				ImGui::CheckboxFill(XORSTR("VR-Mode"), &localFlags[17]);
				// ImGui::Separator();
			}
			ImGui::EndChild();

			ImGui::NextColumn();
			{
				ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 226);
				ImGui::BeginChild(XORSTR("##PlayerVisuals2"), ImVec2(0, 736), true);
				{
					// ImGui::Separator();
					// ImGui::Columns(1);
					ImGui::Text(XORSTR("Customizations"));
					ImGui::Separator();
					ImGui::CheckboxFill(XORSTR("EditMode"), &Settings::MaterialConfig::config.bEditMode);
					// proxiesTestMode
					ImGui::CheckboxFill(XORSTR("Compressed Textures"),
									&Settings::MaterialConfig::config.bCompressedTextures);
					ImGui::CheckboxFill(XORSTR("Filter Lightmaps"), &Settings::MaterialConfig::config.bFilterLightmaps);
					ImGui::CheckboxFill(XORSTR("Filter Textures"), &Settings::MaterialConfig::config.bFilterTextures);
					ImGui::CheckboxFill(XORSTR("Reverse Depth"), &Settings::MaterialConfig::config.bReverseDepth);
					ImGui::CheckboxFill(XORSTR("Buffer Primitives"), &Settings::MaterialConfig::config.bBufferPrimitives);
					ImGui::CheckboxFill(XORSTR("Draw Flat"), &Settings::MaterialConfig::config.bDrawFlat);
					ImGui::CheckboxFill(XORSTR("Measure Fill-Rate"), &Settings::MaterialConfig::config.bMeasureFillRate);
					ImGui::CheckboxFill(XORSTR("Visualize Fill-Rate"),
									&Settings::MaterialConfig::config.bVisualizeFillRate);
					ImGui::CheckboxFill(XORSTR("No Transparency"), &Settings::MaterialConfig::config.bNoTransparency);
					ImGui::CheckboxFill(XORSTR("Software Lighting"),
									&Settings::MaterialConfig::config.bSoftwareLighting); // Crashes game
																			    // AllowCheats ?
					ImGui::PushItemWidth(-1);

					ImGui::SliderInt(XORSTR("##MIPLEVELS"), (int *)&Settings::MaterialConfig::config.nShowMipLevels, 0,
								  3, XORSTR("ShowMipLevels: %1.f"));
					ImGui::PopItemWidth();

					ImGui::CheckboxFill(XORSTR("Show Low-Res Image"), &Settings::MaterialConfig::config.bShowLowResImage);
					ImGui::CheckboxFill(XORSTR("Show Normal Map"), &Settings::MaterialConfig::config.bShowNormalMap);
					ImGui::CheckboxFill(XORSTR("MipMap Textures"), &Settings::MaterialConfig::config.bMipMapTextures);
					ImGui::PushItemWidth(-1);

					ImGui::SliderInt(XORSTR("##NFULLBRIGHT"), (int *)&Settings::MaterialConfig::config.nFullbright, 0, 3,
								  XORSTR("nFullBright: %1.f"));
					ImGui::PopItemWidth();

					SetTooltip(XORSTR("1 = Bright World, 2 = Bright Models"));
					ImGui::CheckboxFill(XORSTR("Fast NoBump"), &Settings::MaterialConfig::config.m_bFastNoBump);
					ImGui::CheckboxFill(XORSTR("Suppress Rendering"),
									&Settings::MaterialConfig::config.m_bSuppressRendering);
					ImGui::CheckboxFill(XORSTR("Draw Gray"), &Settings::MaterialConfig::config.m_bDrawGray);
					ImGui::CheckboxFill(XORSTR("Show Specular"), &Settings::MaterialConfig::config.bShowSpecular);
					ImGui::CheckboxFill(XORSTR("Show Defuse"), &Settings::MaterialConfig::config.bShowDiffuse);
					// ImGui::Separator();
				}
				ImGui::EndChild();
			}
			ImGui::NextColumn();
			{
				ImGui::BeginChild(XORSTR("##PlayerVisuals3"), ImVec2(0, 736), true);
				{
					// ImGui::Separator();
					// ImGui::Columns(1);
					ImGui::Text(XORSTR("Adjustable Display Settings"));
					ImGui::Separator();
					ImGui::CheckboxFill(XORSTR("GammaTVEnabled"), &Settings::MaterialConfig::config.m_bGammaTVEnabled);
					ImGui::PushItemWidth(-1);

					ImGui::SliderFloat(XORSTR("##MONITORGAMMA"), &Settings::MaterialConfig::config.m_fMonitorGamma, 0.1f,
								    12.0f,
								    XORSTR("Gamma: %.3f"));
					ImGui::SliderFloat(XORSTR("##GAMMATVRANGEMIN"), &Settings::MaterialConfig::config.m_fGammaTVRangeMin,
								    0.1f, std::min(300.0f, Settings::MaterialConfig::config.m_fGammaTVRangeMax),
								    XORSTR("TVRangeMin: %.3f"));
					ImGui::SliderFloat(XORSTR("##GAMMATVRANGEMAX"), &Settings::MaterialConfig::config.m_fGammaTVRangeMax,
								    Settings::MaterialConfig::config.m_fGammaTVRangeMin, 300.0f,
								    XORSTR("TVRangeMax: %.3f"));
					ImGui::SliderFloat(XORSTR("##GAMMATVEXPONENT"), &Settings::MaterialConfig::config.m_fGammaTVExponent,
								    0.1f, 3.0f, XORSTR("TV Exponent: %.3f"));
					ImGui::PopItemWidth();

					ImGui::Separator();
					ImGui::Columns(1);
					ImGui::Text(XORSTR("Options"));
					ImGui::Separator();
					ImGui::CheckboxFill(XORSTR("Shadow Depth Texture"),
									&Settings::MaterialConfig::config.m_bShadowDepthTexture);
					SetTooltip(XORSTR("Risky. May cause black Screen. Reset if it does."));
					ImGui::CheckboxFill(XORSTR("Motion Blur"), &Settings::MaterialConfig::config.m_bMotionBlur);
					ImGui::CheckboxFill(XORSTR("Support Flashlight"),
									&Settings::MaterialConfig::config.m_bSupportFlashlight);
					ImGui::CheckboxFill(XORSTR("Paint Enabled"), &Settings::MaterialConfig::config.m_bPaintEnabled);

					ImGui::Separator();
					ImGui::Columns(1);
					ImGui::Text(XORSTR("Customize AntiAliasing"));
					ImGui::Separator();
					ImGui::CheckboxFill(XORSTR("TripleBuffered"), &Settings::MaterialConfig::config.m_bTripleBuffered);
					ImGui::PushItemWidth(-1);

					ImGui::SliderInt(XORSTR("##AASAMPLES"), &Settings::MaterialConfig::config.m_nAASamples, 0, 16,
								  XORSTR("AA Samples: %1.f"));
					ImGui::SliderInt(XORSTR("##FORCEANISOTROPICLEVEL"),
								  &Settings::MaterialConfig::config.m_nForceAnisotropicLevel, 0, 8,
								  XORSTR("Anisotropic Level: %1.f"));
					ImGui::SliderInt(XORSTR("##SKIPMIPLEVELS"), &Settings::MaterialConfig::config.skipMipLevels, 0, 10,
								  XORSTR("SkipMipLevels: %1.f"));
					SetTooltip(XORSTR("Makes the game flatter."));
					ImGui::SliderInt(XORSTR("##AAQUALITY"), &Settings::MaterialConfig::config.m_nAAQuality, 0, 16,
								  XORSTR("AAQuality: %1.f"));
					ImGui::PopItemWidth();

					ImGui::Separator();
					static bool localFlags[] = {
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_WINDOWED) != 0, // ( 1 << 0 )
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_RESIZING) != 0, // ( 1 << 1 )
					    false,												  // ( 1 << 2 ) is not used.
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_NO_WAIT_FOR_VSYNC) != 0, // ( 1 << 3 )
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_STENCIL) != 0, // ...
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_TRILINEAR) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_HWSYNC) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_SPECULAR) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_BUMPMAP) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_PARALLAX_MAPPING) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USE_Z_PREFILL) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_REDUCE_FILLRATE) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_HDR) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_LIMIT_WINDOWED_SIZE) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_SCALE_TO_OUTPUT_RESOLUTION) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USING_MULTIPLE_WINDOWS) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_PHONG) != 0,
					    (Settings::MaterialConfig::config.m_Flags &
						MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_VR_MODE) != 0,
					};
					ImGui::CheckboxFill(XORSTR("Enable Changes"), &Settings::MaterialConfig::enabled);
					SetTooltip(
					    XORSTR("Expect some lag when changing these settings.\nIf your hud breaks, toggle cl_drawhud off/on"));
					if (ImGui::Button(XORSTR("Reset Changes")))
					{
						Settings::MaterialConfig::config = MaterialConfig::backupConfig;
					}
					if (ImGui::Button(XORSTR("Apply ")))
					{
						for (unsigned short i = 0; i < 18; i++)
						{
							if (i == 2)
							{ // ( 1 << 2 ) not used.
								continue;
							}
							Settings::MaterialConfig::config.SetFlag((unsigned int)(1 << i), localFlags[i]);
						}
					}
					// ImGui::Separator();
				}
				ImGui::EndChild();
			}
			ImGui::EndColumns();
		}
		ImGui::EndGroup();
	}
}

void VisualsESP::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
	{
		ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::ESP::enabled);
	}
	ImGui::EndGroup();

	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
		ImGui::Columns(3, nullptr, false);
		{
			ImGui::SetColumnOffset(1, 500);
			ImGui::BeginChild(XORSTR("##Visuals1"), ImVec2(0, 268), true);
			{
				ImGui::Separator();
				ImGui::Text("Allies ESP");
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Outline Box"), &Settings::ESP::FilterAlise::Boxes::enabled);
					ImGui::Checkbox(XORSTR("Chams"), &Settings::ESP::FilterAlise::Chams::enabled);
					ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::FilterAlise::HealthBar::enabled);
					ImGui::Checkbox(XORSTR("Tracers"), &Settings::ESP::FilterAlise::Tracers::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::Combo(XORSTR("##BOXTYPE"), (int *)&Settings::ESP::FilterAlise::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
					ImGui::Combo(XORSTR("##CHAMSTYPE"), (int *)&Settings::ESP::FilterAlise::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
					ImGui::Combo(XORSTR("##BARTYPE"), (int *)&Settings::ESP::FilterAlise::HealthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
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
			ImGui::EndChild();
		}
		ImGui::NextColumn();
		{
			ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 226);
			ImGui::BeginChild(XORSTR("##Visuals2"), ImVec2(0, 268), true);
			{
				ImGui::Separator();
				ImGui::Text("Localplayer ESP");
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Box"), &Settings::ESP::FilterLocalPlayer::Boxes::enabled);
					ImGui::Checkbox(XORSTR("FakeChams"), &Settings::ESP::FilterLocalPlayer::Chams::enabled);
					ImGui::Checkbox(XORSTR("RealChams"), &Settings::ESP::FilterLocalPlayer::RealChams::enabled);
					ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::FilterLocalPlayer::HealthBar::enabled);
					ImGui::Checkbox(XORSTR("Tracers"), &Settings::ESP::FilterLocalPlayer::Tracers::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::Combo(XORSTR("##BOXTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
					ImGui::Combo(XORSTR("##FakeCHAMSTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
					ImGui::Combo(XORSTR("##RealCHAMSTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::RealChams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
					ImGui::Combo(XORSTR("##BARTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::HealthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
					ImGui::Combo(XORSTR("##TRACERTYPE"), (int *)&Settings::ESP::FilterLocalPlayer::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
					ImGui::PopItemWidth();
				}
				ImGui::EndColumns();
				ImGui::Columns(1, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Player Info"), &Settings::ESP::FilterLocalPlayer::playerInfo::enabled);
					ImGui::Checkbox(XORSTR("Skeleton"), &Settings::ESP::FilterLocalPlayer::Skeleton::enabled);
					ImGui::Checkbox(XORSTR("Glow"), &Settings::ESP::Glow::enabled);
					ImGui::Checkbox(XORSTR("Draw AA Trace"), &Settings::ESP::DrawAATrace::enabled);
					ImGui::Checkbox(XORSTR("Sync fake chams with fakelag"), &Settings::ESP::SyncFake);
				}
			}
			ImGui::EndChild();
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("##Visuals3"), ImVec2(0, 268), true);
			{
				ImGui::Separator();
				ImGui::Text("Enemy ESP");
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Box"), &Settings::ESP::FilterEnemy::Boxes::enabled);
					ImGui::Checkbox(XORSTR("Chams"), &Settings::ESP::FilterEnemy::Chams::enabled);
					ImGui::Checkbox(XORSTR("Health Bar"), &Settings::ESP::FilterEnemy::HealthBar::enabled);
					ImGui::Checkbox(XORSTR("Tracers"), &Settings::ESP::FilterEnemy::Tracers::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::Combo(XORSTR("##BOXTYPE"), (int *)&Settings::ESP::FilterEnemy::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
					ImGui::Combo(XORSTR("##CHAMSTYPE"), (int *)&Settings::ESP::FilterEnemy::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
					ImGui::Combo(XORSTR("##BARTYPE"), (int *)&Settings::ESP::FilterEnemy::HealthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
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
					ImGui::Checkbox(XORSTR("Draw Backtrack Chams"), &Settings::ESP::FilterEnemy::Chams::drawBacktrack);
				}
				ImGui::SameLine();
				ColorButton::RenderWindow("Enemy Chams Visible", (int)50, ImGui::ColorButton(XORSTR("Chams - Enemy Visible"), (ImVec4)Settings::ESP::enemyVisibleColor.color, 0, ImVec2(19, 19)));
				ImGui::SameLine();
				ColorButton::RenderWindow("Enemy Chams Hidden", (int)81, ImGui::ColorButton(XORSTR("Chams - Enemy Hidden"), (ImVec4)Settings::ESP::enemyColor.color, 0, ImVec2(20, 20)));
			}
			ImGui::EndChild();
		}
		ImGui::EndColumns();
		ImGui::Columns(2, nullptr, false);
		{
			ImGui::BeginChild(XORSTR("#PlayerInfo"), ImVec2(0, 268), true);
			{
				ImGui::Separator();
				ImGui::Text("Player Info Customizations");
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Clan"), &Settings::ESP::Info::clan);
					ImGui::Checkbox(XORSTR("Rank"), &Settings::ESP::Info::rank);
					ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::Info::health);
					ImGui::Checkbox(XORSTR("Armor"), &Settings::ESP::Info::armor);
					ImGui::Checkbox(XORSTR("Scoped"), &Settings::ESP::Info::scoped);
					ImGui::Checkbox(XORSTR("Flashed"), &Settings::ESP::Info::flashed);
					ImGui::Checkbox(XORSTR("Defuse Kit"), &Settings::ESP::Info::hasDefuser);
					ImGui::Checkbox(XORSTR("Grabbing Hostage"), &Settings::ESP::Info::grabbingHostage);
					ImGui::Checkbox(XORSTR("Location"), &Settings::ESP::Info::location);
					ImGui::Checkbox(XORSTR("Name"), &Settings::ESP::Info::name);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(XORSTR("Steam ID"), &Settings::ESP::Info::steamId);
					ImGui::Checkbox(XORSTR("Weapon"), &Settings::ESP::Info::weapon);
					ImGui::Checkbox(XORSTR("FakeDuck"), &Settings::ESP::Info::Fakeduck);
					ImGui::Checkbox(XORSTR("Reloading"), &Settings::ESP::Info::reloading);
					ImGui::Checkbox(XORSTR("Planting"), &Settings::ESP::Info::planting);
					ImGui::Checkbox(XORSTR("Defusing"), &Settings::ESP::Info::defusing);
					ImGui::Checkbox(XORSTR("Resolver Flags"), &Settings::ESP::Info::rescuing);
					ImGui::Checkbox(XORSTR("Layers Debug"), &Settings::Debug::AnimLayers::draw);
					ImGui::Checkbox(XORSTR("Choked Packets"), &Settings::ESP::Info::money);
				}
				ImGui::EndColumns();
			}
			ImGui::EndChild();
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("#ESSP"), ImVec2(0, 268), false);
			{
				// SOON
			}
			ImGui::EndChild();
		}
		ImGui::EndColumns();
	}
	ImGui::EndGroup();
}
void VisualsLocal::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
	{
		ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::ESP::enabled);
	}
	ImGui::EndGroup();

	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
		ImGui::Columns(2, nullptr, false);
		{
			ImGui::BeginChild(XORSTR("##Visuals1"), ImVec2(0, 736), true);
			{
				ImGui::Separator();
				ImGui::Text("Danger Zone");
				ImGui::Separator();
				// This is retarded way to do things nice, but if it works it works!
				ImGui::Columns(4, nullptr, false);
				{
					ImGui::SetColumnOffset(1, 150);
					ImGui::Checkbox(XORSTR("Loot Crates"), &Settings::ESP::DangerZone::lootcrate);
					ImGui::Checkbox(XORSTR("Weapon Upgrades"), &Settings::ESP::DangerZone::upgrade);
					ImGui::Checkbox(XORSTR("Ammo box"), &Settings::ESP::DangerZone::ammobox);
					ImGui::Checkbox(XORSTR("Radar Jammer"), &Settings::ESP::DangerZone::radarjammer);
					ImGui::Checkbox(XORSTR("Safe"), &Settings::ESP::DangerZone::safe);
					ImGui::Checkbox(XORSTR("Sentry Turret"), &Settings::ESP::DangerZone::dronegun);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(2, 235);

					ColorButton::RenderWindow("Loot Crate", (int)36, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Loot Crate"), (ImVec4)Settings::ESP::DangerZone::lootcrateColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Weapon Upgrade", (int)35, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Weapon Upgrade"), (ImVec4)Settings::ESP::DangerZone::upgradeColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Ammo Box", (int)39, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Ammo Box"), (ImVec4)Settings::ESP::DangerZone::ammoboxColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Radar Jammer", (int)37, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Radar Jammer"), (ImVec4)Settings::ESP::DangerZone::radarjammerColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Safe", (int)40, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Safe"), (ImVec4)Settings::ESP::DangerZone::safeColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Sentry Turret", (int)41, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Sentry Turret"), (ImVec4)Settings::ESP::DangerZone::dronegunColor.color, 0, ImVec2(19, 19)));
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(3, 400);
					ImGui::Checkbox(XORSTR("Melee"), &Settings::ESP::DangerZone::melee);
					ImGui::Checkbox(XORSTR("Tablet"), &Settings::ESP::DangerZone::tablet);
					ImGui::Checkbox(XORSTR("Cash"), &Settings::ESP::DangerZone::cash);
					ImGui::Checkbox(XORSTR("Drone"), &Settings::ESP::DangerZone::drone);
					ImGui::Checkbox(XORSTR("Healthshot"), &Settings::ESP::DangerZone::healthshot);
					ImGui::Checkbox(XORSTR("Explosive Barrel"), &Settings::ESP::DangerZone::barrel);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(4, 490);
					ColorButton::RenderWindow("Melee", (int)46, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Melee"), (ImVec4)Settings::ESP::DangerZone::meleeColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Tablet", (int)44, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Tablet"), (ImVec4)Settings::ESP::DangerZone::tabletColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Cash", (int)43, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Cash"), (ImVec4)Settings::ESP::DangerZone::cashColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Drone", (int)42, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Drone"), (ImVec4)Settings::ESP::DangerZone::droneColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Healthshot", (int)45, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Healthshot"), (ImVec4)Settings::ESP::DangerZone::healthshotColor.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Explosive Barrel", (int)38, ImGui::ColorButton(XORSTR("ESP - Danger Zone: Explosive Barrel"), (ImVec4)Settings::ESP::DangerZone::barrelColor.color, 0, ImVec2(19, 19)));
				}
				ImGui::Columns(1);
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Draw Distance"), &Settings::ESP::DangerZone::drawDistEnabled);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderInt(XORSTR("##DZDRAWDIST"), &Settings::ESP::DangerZone::drawDist, 1, 10000, XORSTR("Amount: %0.f"));
				}
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Crosshair Options");
				ImGui::Separator();
				ImGui::Columns(4, nullptr, false);
				{
					ImGui::SetColumnOffset(1, 150);
					ImGui::Checkbox(XORSTR("Show Spread"), &Settings::ESP::Spread::enabled);
					ImGui::Checkbox(XORSTR("FOV Circle"), &Settings::ESP::FOVCrosshair::enabled);
					ImGui::Checkbox(XORSTR("Recoil Crosshair"), &Settings::Recoilcrosshair::enabled);
					ImGui::Checkbox(XORSTR("Velocity Graph"), &Settings::ESP::VelGraph);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(2, 235);
					ColorButton::RenderWindow("Spread", (int)5, ImGui::ColorButton(XORSTR("Spread"), (ImVec4)Settings::ESP::Spread::color.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("FOV Circle", (int)4, ImGui::ColorButton(XORSTR("FOV Crosshair"), (ImVec4)Settings::ESP::FOVCrosshair::color.color, 0, ImVec2(19, 19)));
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(3, 400);
					ImGui::Checkbox(XORSTR("Show SpreadLimit"), &Settings::ESP::Spread::spreadLimit);
					ImGui::Checkbox(XORSTR("Only When Shooting"), &Settings::Recoilcrosshair::showOnlyWhenShooting);
					ImGui::Checkbox(XORSTR("Filled"), &Settings::ESP::FOVCrosshair::filled);
					ImGui::Checkbox(XORSTR("Show Indicators"), &Settings::ESP::indicators::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(4, 490);
					ColorButton::RenderWindow("Spreadlimit", (int)6, ImGui::ColorButton(XORSTR("Spreadlimit"), (ImVec4)Settings::ESP::Spread::spreadLimitColor.color, 0, ImVec2(19, 19)));
				}
				ImGui::EndColumns();
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
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderFloat(XORSTR("##RADARZOOM"), &Settings::Radar::zoom, 0.f, 100.f, XORSTR("Zoom: %0.f"));
					ImGui::SliderFloat(XORSTR("##RADARICONSSCALE"), &Settings::Radar::iconsScale, 2, 16, XORSTR("Icons Scale: %0.1f"));
					ImGui::PopItemWidth();
				}
				ImGui::EndColumns();
				ImGui::Columns(4, nullptr, false);
				{
					ImGui::SetColumnOffset(1, 150);
					ImGui::Checkbox(XORSTR("Enemies"), &Settings::Radar::enemies);
					ImGui::Checkbox(XORSTR("Bomb"), &Settings::Radar::bomb);
					ImGui::Spacing();
					ImGui::Text("CT - Color");
					ImGui::Spacing();
					ImGui::Checkbox(XORSTR("Legit"), &Settings::Radar::legit);
					ImGui::Checkbox(XORSTR("In-game Radar"), &Settings::Radar::InGame::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(2, 235);
					ColorButton::RenderWindow("Radar - Enemy", (int)62, ImGui::ColorButton(XORSTR("Radar - Enemy"), (ImVec4)Settings::Radar::enemyColor.color, 0, ImVec2(19, 19)));
					ImGui::SameLine();
					ColorButton::RenderWindow("Radar - Enemy Visible", (int)64, ImGui::ColorButton(XORSTR("Radar - Enemy Visible"), (ImVec4)Settings::Radar::enemyVisibleColor.color, 0, ImVec2(19, 19)));

					ColorButton::RenderWindow("Radar - Bomb", (int)70, ImGui::ColorButton(XORSTR("Radar - Bomb"), (ImVec4)Settings::Radar::bombColor.color, 0, ImVec2(19, 19)));
					ImGui::SameLine();
					ColorButton::RenderWindow("Radar - Bomb Defusing", (int)71, ImGui::ColorButton(XORSTR("Radar - Bomb Defusing"), (ImVec4)Settings::Radar::bombDefusingColor.color, 0, ImVec2(19, 19)));

					ColorButton::RenderWindow("Radar - CT", (int)66, ImGui::ColorButton(XORSTR("Radar - CT"), (ImVec4)Settings::Radar::ctColor.color, 0, ImVec2(19, 19)));
					ImGui::SameLine();
					ColorButton::RenderWindow("Radar - CT Visible", (int)68, ImGui::ColorButton(XORSTR("Radar - CT Visible"), (ImVec4)Settings::Radar::ctVisibleColor.color, 0, ImVec2(19, 19)));
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(3, 400);
					ImGui::Checkbox(XORSTR("Allies"), &Settings::Radar::allies);
					ImGui::Spacing();
					ImGui::Text("T - Color");
					ImGui::Spacing();
					ImGui::Checkbox(XORSTR("Defuser"), &Settings::Radar::defuser);
					ImGui::Checkbox(XORSTR("Visibility Check"), &Settings::Radar::visibilityCheck);
					ImGui::Checkbox(XORSTR("Smoke Check"), &Settings::Radar::smokeCheck);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(4, 490);
					ColorButton::RenderWindow("Radar - Team", (int)63, ImGui::ColorButton(XORSTR("Radar - Team"), (ImVec4)Settings::Radar::allyColor.color, 0, ImVec2(19, 19)));
					ImGui::SameLine();
					ColorButton::RenderWindow("Radar - Team Visible", (int)65, ImGui::ColorButton(XORSTR("Radar - Team Visible"), (ImVec4)Settings::Radar::allyVisibleColor.color, 0, ImVec2(19, 19)));

					ColorButton::RenderWindow("Radar - T", (int)67, ImGui::ColorButton(XORSTR("Radar - T"), (ImVec4)Settings::Radar::tColor.color, 0, ImVec2(19, 19)));
					ImGui::SameLine();
					ColorButton::RenderWindow("Radar - T Visible", (int)69, ImGui::ColorButton(XORSTR("Radar - T Visible"), (ImVec4)Settings::Radar::tVisibleColor.color, 0, ImVec2(19, 19)));
				}
				ImGui::EndColumns();
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text(XORSTR("Event logger"));
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Show Enemies"), &Settings::Eventlog::showEnemies);
					ImGui::Checkbox(XORSTR("Show Allies"), &Settings::Eventlog::showTeammates);
					ImGui::Checkbox(XORSTR("Show LocalPlayer"), &Settings::Eventlog::showLocalplayer);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderFloat(XORSTR("##LOGGERDURATION"), &Settings::Eventlog::duration, 1000.f, 5000.f, XORSTR("Log duration: %0.f"));
					ImGui::SliderFloat(XORSTR("##LOGGERLINES"), &Settings::Eventlog::lines, 5, 15, XORSTR("Log lines: %0.f"));
					ImGui::Spacing();
					ImGui::Text("Event Log Color");
					ImGui::SameLine();
					ColorButton::RenderWindow("Event log", (int)61, ImGui::ColorButton(XORSTR("Event log"), (ImVec4)Settings::Eventlog::color.color, 0, ImVec2(19, 19)));
					ImGui::PopItemWidth();
				}
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text(XORSTR("Tracer Effect"));
				ImGui::Separator();
				ImGui::PushItemWidth(-1);
				if (Settings::TracerEffects::serverSide)
				{
					Settings::TracerEffects::frequency = 1;
					Settings::TracerEffects::effect = TracerEffects_t::TASER;
				}
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Combo(XORSTR("##TracerEffects"), (int *)&Settings::TracerEffects::effect, tracerEffectNames, IM_ARRAYSIZE(tracerEffectNames));
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderInt(XORSTR("##TracerFreq"), &Settings::TracerEffects::frequency, 0, 10, XORSTR("Freq: %0.f"));
				}
				ImGui::EndColumns();
				ImGui::Columns(3, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Enable Tracers"), &Settings::TracerEffects::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(XORSTR("Server Sided?"), &Settings::TracerEffects::serverSide);
					SetTooltip(XORSTR("Requires a Taser in your Inventory.\nCan only shoot one shot at a time\nOnly Works with Kisak Snot"));
				}
				ImGui::NextColumn();
				{
					if (ImGui::Button(XORSTR("Restore Tracers")))
					{
						TracerEffect::RestoreTracers();
					}
				}
				ImGui::EndColumns();
			}
			ImGui::EndChild();
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("##Visuals2"), ImVec2(0, 736), true);
			{
				ImGui::Separator();
				ImGui::Text("ESP Features");
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Skybox Changer"), &Settings::SkyBox::enabled);
					ImGui::Checkbox(XORSTR("Sleeves"), &Settings::ESP::Chams::Sleeves::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::Combo(XORSTR("##SKYBOX"), &Settings::SkyBox::skyBoxNumber, SkyBoxes, IM_ARRAYSIZE(SkyBoxes));
					ImGui::Combo(XORSTR("##SLEEVESTYPE"), (int *)&Settings::ESP::Chams::Sleeves::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				}
				ImGui::EndColumns();
				ImGui::Columns(4, nullptr, false);
				{
					ImGui::SetColumnOffset(1, 150);

					ImGui::Checkbox(XORSTR("Arms"), &Settings::ESP::Chams::Arms::enabled);
					ImGui::Checkbox(XORSTR("Weapons"), &Settings::ESP::Chams::Weapon::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(2, 228);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(3, 420);
					ImGui::PushItemWidth(-1);
					ImGui::Combo(XORSTR("##ARMSTYPE"), (int *)&Settings::ESP::Chams::Arms::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
					ImGui::Combo(XORSTR("##WEAPONTYPE"), (int *)&Settings::ESP::Chams::Weapon::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(4, 580);
					ColorButton::RenderWindow("Chams - Arms", (int)52, ImGui::ColorButton(XORSTR("Chams - Arms"), (ImVec4)Settings::ESP::Chams::Arms::color.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Chams - Weapon", (int)53, ImGui::ColorButton(XORSTR("Chams - Weapon"), (ImVec4)Settings::ESP::Chams::Weapon::color.color, 0, ImVec2(19, 19)));
				}
				ImGui::EndColumns();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Dlights"), &Settings::Dlights::enabled);
					ImGui::Checkbox(XORSTR("No Flash"), &Settings::Noflash::enabled);
					ImGui::Checkbox(XORSTR("No Smoke"), &Settings::NoSmoke::enabled);
					ImGui::Checkbox(XORSTR("Show Footsteps"), &Settings::ESP::Sounds::enabled);
					ImGui::Checkbox(XORSTR("Nightmode"), &Settings::Nightmode::enabled);
					ImGui::Checkbox(XORSTR("No View Punch"), &Settings::View::NoViewPunch::enabled);
					ImGui::Checkbox(XORSTR("Show Hitbox impacts"), &Settings::ESP::showimpacts);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderFloat(XORSTR("##DLIGHTRADIUS"), &Settings::Dlights::radius, 0, 1000, XORSTR("Radius: %0.f"));
					ImGui::SliderFloat(XORSTR("##NOFLASHAMOUNT"), &Settings::Noflash::value, 0, 255, XORSTR("Amount: %0.f"));
					ImGui::Combo(XORSTR("##SMOKETYPE"), (int *)&Settings::NoSmoke::type, SmokeTypes, IM_ARRAYSIZE(SmokeTypes));
					ImGui::SliderInt(XORSTR("##SOUNDSTIME"), &Settings::ESP::Sounds::time, 250, 5000, XORSTR("Timeout: %0.f"));
					ImGui::SliderInt(XORSTR("##NIGHTMODE"), &Settings::Nightmode::value, 0, 100, XORSTR("Nightmode: %0.f"));
					ImGui::PopItemWidth();
					ImGui::Checkbox(XORSTR("Draw Molotov"), &Settings::ESP::Drawfire::enabled);
					ImGui::Checkbox(XORSTR("No Aim Punch"), &Settings::View::NoAimPunch::enabled);
					ImGui::Checkbox(XORSTR("Draw Taser Range"), &Settings::ESP::taserrange::enabled);
				}
				ImGui::EndColumns();
				ImGui::Columns(4, nullptr, false);
				{
					ImGui::SetColumnOffset(1, 150);
					ImGui::Checkbox(XORSTR("No Sky"), &Settings::NoSky::enabled);
					ImGui::Checkbox(XORSTR("ASUS Walls"), &Settings::ASUSWalls::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(2, 228);
					ColorButton::RenderWindow("Sky", (int)80, ImGui::ColorButton(XORSTR("Sky"), (ImVec4)Settings::NoSky::color.color, 0, ImVec2(19, 19)));
					ColorButton::RenderWindow("Walls", (int)81, ImGui::ColorButton(XORSTR("Walls"), (ImVec4)Settings::ASUSWalls::color.color, 0, ImVec2(19, 19)));
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(3, 420);
					ImGui::Checkbox(XORSTR("Grenade Prediction"), &Settings::GrenadePrediction::enabled);
					ImGui::Checkbox(XORSTR("No Scope Border"), &Settings::NoScopeBorder::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::SetColumnOffset(4, 580);
					ColorButton::RenderWindow("Grenade Prediction - Line", (int)60, ImGui::ColorButton(XORSTR("Grenade Prediction - Line"), (ImVec4)Settings::GrenadePrediction::color.color, 0, ImVec2(19, 19)));
				}
				ImGui::EndColumns();
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Draw Bullet Tracers"), &Settings::ESP::tracebullet::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(XORSTR("Localplayer only"), &Settings::ESP::tracebullet::local);
				}
				ImGui::EndColumns();
				ImGui::Separator();
				ImGui::Text(XORSTR("Debug"));
				ImGui::Separator();
				ImGui::Columns(3, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("BoneMap Debug"), &Settings::Debug::BoneMap::draw);
					ImGui::Checkbox(XORSTR("AimSpot Debug"), &Settings::Debug::AutoAim::drawTarget);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(XORSTR("Just Dots"), &Settings::Debug::BoneMap::justDrawDots);
					ImGui::Checkbox(XORSTR("Autowall Debug"), &Settings::Debug::AutoWall::debugView);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderInt(XORSTR("##BoneMap"), &Settings::Debug::BoneMap::modelID, 1253, 1350, XORSTR("Model ID: %0.f"));
				}
				ImGui::EndColumns();
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text("Filter World Items");
				ImGui::Separator();
				ImGui::Columns(3, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Weapon ESP"), &Settings::ESP::Filters::weapons);
					ImGui::Checkbox(XORSTR("Throwables"), &Settings::ESP::Filters::throwables);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(XORSTR("Bomb"), &Settings::ESP::Filters::bomb);
					ImGui::Checkbox(XORSTR("Defuse Kits"), &Settings::ESP::Filters::defusers);
				}
				ImGui::NextColumn();
				{
					ImGui::Checkbox(XORSTR("Hostages"), &Settings::ESP::Filters::hostages);
				}
				ImGui::EndColumns();
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text(XORSTR("Hitmarkers"));
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Checkbox(XORSTR("Hitmarkers"), &Settings::ESP::Hitmarker::enabled);
					ImGui::Checkbox(XORSTR("Enemies##HITMARKERS"), &Settings::ESP::Hitmarker::enemies);
					ImGui::Checkbox(XORSTR("Allies##HITMARKERS"), &Settings::ESP::Hitmarker::allies);
					ImGui::Checkbox(XORSTR("Sounds##HITMARKERS"), &Settings::ESP::Hitmarker::Sounds::enabled);
					ImGui::Checkbox(XORSTR("Damage##HITMARKERS"), &Settings::ESP::Hitmarker::Damage::enabled);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderInt(XORSTR("##HITMARKERDUR"), &Settings::ESP::Hitmarker::duration, 250, 3000, XORSTR("Timeout: %0.f"));
					ImGui::SliderInt(XORSTR("##HITMARKERSIZE"), &Settings::ESP::Hitmarker::size, 1, 32, XORSTR("Size: %0.f"));
					ImGui::SliderInt(XORSTR("##HITMARKERGAP"), &Settings::ESP::Hitmarker::innerGap, 1, 16, XORSTR("Gap: %0.f"));
					ImGui::Combo(XORSTR("##SOUNDS"), (int *)&Settings::ESP::Hitmarker::Sounds::sound, Sounds, IM_ARRAYSIZE(Sounds));
					ImGui::Spacing();
					ImGui::Text("Hitmarker Color");
					ImGui::SameLine();
					ColorButton::RenderWindow("Hitmarker", (int)7, ImGui::ColorButton(XORSTR("Hitmarker"), (ImVec4)Settings::ESP::Hitmarker::color.color, 0, ImVec2(19, 19)));
				}
				ImGui::EndColumns();
				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::Text(XORSTR("OOV Arrow Settings"));
				ImGui::Separator();
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::SliderInt(XORSTR("##ARROWDISTANCE"), &Settings::ESP::arrows::distance, 1, 100, XORSTR("Arrows distance: %0.f"));
				}
				ImGui::NextColumn();
				{
					ImGui::SliderInt(XORSTR("##ARROWSIZE"), &Settings::ESP::arrows::size, 1, 100, XORSTR("Arrows size: %0.f"));
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndColumns();
	}
	ImGui::EndGroup();
}

void VisualsMenu::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 945, pos.y + 95), ImColor(0, 0, 0, 150), 10);
	ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
	{
		ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::ESP::enabled);
	}
	ImGui::EndGroup();

	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
		ImGui::Columns(2, nullptr, false);
		{
			ImGui::BeginChild(XORSTR("##Visuals2"), ImVec2(0, 736), true);
			{
			}
			ImGui::EndChild();
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("##Visuals2"), ImVec2(0, 736), true);
			{
			}
			ImGui::EndChild();
		}
		ImGui::EndColumns();
	}
}