#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wformat-security"

#include "hvhtab.h"

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Features/valvedscheck.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

void HvH::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
    static int currentType;
    static char *slowType[] = {
        "Safety", "Accuracy",
    };

    static const char* movementType[] = {
        "Stand", "Move", "Air", "SlowWalk",
    };

    static const char* yawType[] = {
        "None", "Jitter", "RandomJitter", "SPIN",
    };

    static const char* pitchType[] = {
        "None", "UP", "DOWN", "DANCE", "CUSTOM",
    };

    static const char* desyncType[] = {
        "None", "Static", "Jitter", "RandomJitter",
    };

    ImGui::BeginGroup();
    {
        ImGui::Columns(2, nullptr, false);
        {
            ImGui::BeginChild(XORSTR("##AntiAim1"), ImVec2(0, 736), true);
            {
                ImGui::Checkbox("AntiAim", &Settings::AntiAim::enabled);
                ImGui::SameLine();
                ImGui::PushItemWidth(-1);
                ImGui::Combo(XORSTR("##MOVTYPES"), (int*)&currentType, movementType, IM_ARRAYSIZE(movementType));
                ImGui::PopItemWidth();
                if (currentType == 0) { // stand
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Yaw"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Pitch"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Desync"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##YAW_STAND"), (int*)&Settings::AntiAim::Stand::Yaw::type, yawType, IM_ARRAYSIZE(yawType));
                        ImGui::Combo(XORSTR("##PITCH_STAND"), (int*)&Settings::AntiAim::Stand::Pitch::type, pitchType, IM_ARRAYSIZE(pitchType));
                        ImGui::Combo(XORSTR("##DESYNC_STAND"), (int*)&Settings::AntiAim::Stand::Desync::type, desyncType, IM_ARRAYSIZE(desyncType));
                        ImGui::PopItemWidth();

                        ImGui::EndColumns();
                    }
                    ImGui::PushItemWidth(-1);
                    if (Settings::AntiAim::Stand::Pitch::type == AntiAimPitch::CUSTOM)
                        ImGui::SliderInt(XORSTR("##PITCH_CUSTOM_1"), &Settings::AntiAim::Stand::Pitch::custom, -89, 89, XORSTR("Pitch: %0.f"));
                    switch (Settings::AntiAim::Stand::Desync::type)
                    {
                        case AntiAimDesync::STATIC:
                            ImGui::SliderInt(XORSTR("##DESYNC_OFFSET_1"), &Settings::AntiAim::Stand::Desync::offset, 0, 58, XORSTR("Desync offset: %0.f"));
                            break;

                        case AntiAimDesync::JITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_1"), &Settings::AntiAim::Stand::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_1"), &Settings::AntiAim::Stand::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right): %0.f"));
                            break;

                        case AntiAimDesync::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_1"), &Settings::AntiAim::Stand::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_MAX_1"), &Settings::AntiAim::Stand::Desync::jitterLeftMax, 0, 58, XORSTR("Desync jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_1"), &Settings::AntiAim::Stand::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_MAX_1"), &Settings::AntiAim::Stand::Desync::jitterRightMax, 1, 58, XORSTR("Desync jitter(right max): %0.f"));
                            break;
                        default: break;
                    }
                    switch(Settings::AntiAim::Stand::Yaw::type)
                    {
                        case AntiAimYaw::JITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_1"), &Settings::AntiAim::Stand::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_1"), &Settings::AntiAim::Stand::Yaw::jitterRight, 0, 58, XORSTR("Yaw jitter(right): %0.f"));
                            break;
                        case AntiAimYaw::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_1"), &Settings::AntiAim::Stand::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_MAX_1"), &Settings::AntiAim::Stand::Yaw::jitterLeftMax, 0, 58, XORSTR("Yaw jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_1"), &Settings::AntiAim::Stand::Yaw::jitterRight, 1, 58, XORSTR("Yaw jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_MAX_1"), &Settings::AntiAim::Stand::Yaw::jitterRightMax, 1, 58, XORSTR("Yaw jitter(right max): %0.f"));
                            break;
                        case AntiAimYaw::SPIN:
                            ImGui::SliderInt(XORSTR("##YAW_SPINF_1"), &Settings::AntiAim::Stand::Yaw::spinFactor, 0, 360, XORSTR("Yaw spinFactor: %0.f"));
                            break;
                        default: break;
                    }
                    ImGui::PopItemWidth();
                } else if (currentType == 1) { // move
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Yaw"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Pitch"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Desync"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##YAW_MOVE"), (int*)&Settings::AntiAim::Movement::Yaw::type, yawType, IM_ARRAYSIZE(yawType));
                        ImGui::Combo(XORSTR("##PITCH_MOVE"), (int*)&Settings::AntiAim::Movement::Pitch::type, pitchType, IM_ARRAYSIZE(pitchType));
                        ImGui::Combo(XORSTR("##DESYNC_MOVE"), (int*)&Settings::AntiAim::Movement::Desync::type, desyncType, IM_ARRAYSIZE(desyncType));
                        ImGui::PopItemWidth();
                        ImGui::EndColumns();
                    }
                    ImGui::PushItemWidth(-1);
                    if (Settings::AntiAim::Movement::Pitch::type == AntiAimPitch::CUSTOM)
                        ImGui::SliderInt(XORSTR("##PITCH_CUSTOM_2"), &Settings::AntiAim::Movement::Pitch::custom, -89, 89, XORSTR("Pitch: %0.f"));
                    switch (Settings::AntiAim::Movement::Desync::type)
                    {
                        case AntiAimDesync::STATIC:
                            ImGui::SliderInt(XORSTR("##DESYNC_OFFSET_2"), &Settings::AntiAim::Movement::Desync::offset, 0, 58, XORSTR("Desync offset: %0.f"));
                            break;

                        case AntiAimDesync::JITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_2"), &Settings::AntiAim::Movement::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_2"), &Settings::AntiAim::Movement::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right): %0.f"));
                            break;

                        case AntiAimDesync::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_2"), &Settings::AntiAim::Movement::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_MAX_2"), &Settings::AntiAim::Movement::Desync::jitterLeftMax, 0, 58, XORSTR("Desync jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_2"), &Settings::AntiAim::Movement::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_MAX_2"), &Settings::AntiAim::Movement::Desync::jitterRightMax, 1, 58, XORSTR("Desync jitter(right max): %0.f"));
                            break;
                        default: break;
                    }
                    switch(Settings::AntiAim::Movement::Yaw::type)
                    {
                        case AntiAimYaw::JITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_2"), &Settings::AntiAim::Movement::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_2"), &Settings::AntiAim::Movement::Yaw::jitterRight, 0, 58, XORSTR("Yaw jitter(right): %0.f"));
                            break;
                        case AntiAimYaw::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_2"), &Settings::AntiAim::Movement::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_MAX_2"), &Settings::AntiAim::Movement::Yaw::jitterLeftMax, 0, 58, XORSTR("Yaw jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_2"), &Settings::AntiAim::Movement::Yaw::jitterRight, 1, 58, XORSTR("Yaw jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_MAX_2"), &Settings::AntiAim::Movement::Yaw::jitterRightMax, 1, 58, XORSTR("Yaw jitter(right max): %0.f"));
                            break;
                        case AntiAimYaw::SPIN:
                            ImGui::SliderInt(XORSTR("##YAW_SPINF_2"), &Settings::AntiAim::Movement::Yaw::spinFactor, 0, 360, XORSTR("Yaw spinFactor: %0.f"));
                            break;
                        default: break;
                    }
                    ImGui::PopItemWidth();
                } else if (currentType == 2) { // air
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Yaw"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Pitch"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Desync"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##YAW_MOVE"), (int*)&Settings::AntiAim::Air::Yaw::type, yawType, IM_ARRAYSIZE(yawType));
                        ImGui::Combo(XORSTR("##PITCH_MOVE"), (int*)&Settings::AntiAim::Air::Pitch::type, pitchType, IM_ARRAYSIZE(pitchType));
                        ImGui::Combo(XORSTR("##DESYNC_MOVE"), (int*)&Settings::AntiAim::Air::Desync::type, desyncType, IM_ARRAYSIZE(desyncType));
                        ImGui::PopItemWidth();
                        ImGui::EndColumns();
                    }
                    ImGui::PushItemWidth(-1);
                    if (Settings::AntiAim::Air::Pitch::type == AntiAimPitch::CUSTOM)
                        ImGui::SliderInt(XORSTR("##PITCH_CUSTOM_3"), &Settings::AntiAim::Air::Pitch::custom, -89, 89, XORSTR("Pitch: %0.f"));
                    switch (Settings::AntiAim::Air::Desync::type)
                    {
                        case AntiAimDesync::STATIC:
                            ImGui::SliderInt(XORSTR("##DESYNC_OFFSET_3"), &Settings::AntiAim::Air::Desync::offset, 0, 58, XORSTR("Desync offset: %0.f"));
                            break;

                        case AntiAimDesync::JITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_3"), &Settings::AntiAim::Air::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_3"), &Settings::AntiAim::Air::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right): %0.f"));
                            break;

                        case AntiAimDesync::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_3"), &Settings::AntiAim::Air::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_MAX_3"), &Settings::AntiAim::Air::Desync::jitterLeftMax, 0, 58, XORSTR("Desync jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_3"), &Settings::AntiAim::Air::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_MAX_3"), &Settings::AntiAim::Air::Desync::jitterRightMax, 1, 58, XORSTR("Desync jitter(right max): %0.f"));
                            break;
                        default: break;
                    }
                    switch(Settings::AntiAim::Air::Yaw::type)
                    {
                        case AntiAimYaw::JITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_3"), &Settings::AntiAim::Air::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_3"), &Settings::AntiAim::Air::Yaw::jitterRight, 0, 58, XORSTR("Yaw jitter(right): %0.f"));
                            break;
                        case AntiAimYaw::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_3"), &Settings::AntiAim::Air::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_MAX_3"), &Settings::AntiAim::Air::Yaw::jitterLeftMax, 0, 58, XORSTR("Yaw jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_3"), &Settings::AntiAim::Air::Yaw::jitterRight, 1, 58, XORSTR("Yaw jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_MAX_3"), &Settings::AntiAim::Air::Yaw::jitterRightMax, 1, 58, XORSTR("Yaw jitter(right max): %0.f"));
                            break;
                        case AntiAimYaw::SPIN:
                            ImGui::SliderInt(XORSTR("##YAW_SPINF_3"), &Settings::AntiAim::Air::Yaw::spinFactor, 0, 360, XORSTR("Yaw spinFactor: %0.f"));
                            break;
                        default: break;
                    }
                    ImGui::PopItemWidth();
                } else if (currentType == 3) { // slowwalk
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Yaw"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Pitch"));
                        ImGui::Spacing();
                        ImGui::Text(XORSTR("Desync"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##YAW_MOVE"), (int*)&Settings::AntiAim::SlowWalk::Yaw::type, yawType, IM_ARRAYSIZE(yawType));
                        ImGui::Combo(XORSTR("##PITCH_MOVE"), (int*)&Settings::AntiAim::SlowWalk::Pitch::type, pitchType, IM_ARRAYSIZE(pitchType));
                        ImGui::Combo(XORSTR("##DESYNC_MOVE"), (int*)&Settings::AntiAim::SlowWalk::Desync::type, desyncType, IM_ARRAYSIZE(desyncType));
                        ImGui::PopItemWidth();
                        ImGui::EndColumns();
                    }
                    ImGui::PushItemWidth(-1);
                    if (Settings::AntiAim::SlowWalk::Pitch::type == AntiAimPitch::CUSTOM)
                        ImGui::SliderInt(XORSTR("##PITCH_CUSTOM_4"), &Settings::AntiAim::SlowWalk::Pitch::custom, -89, 89, XORSTR("Pitch: %0.f"));
                    switch (Settings::AntiAim::SlowWalk::Desync::type)
                    {
                        case AntiAimDesync::STATIC:
                            ImGui::SliderInt(XORSTR("##DESYNC_OFFSET_4"), &Settings::AntiAim::SlowWalk::Desync::offset, 0, 58, XORSTR("Desync offset: %0.f"));
                            break;

                        case AntiAimDesync::JITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_4"), &Settings::AntiAim::SlowWalk::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_4"), &Settings::AntiAim::SlowWalk::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right): %0.f"));
                            break;

                        case AntiAimDesync::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_4"), &Settings::AntiAim::SlowWalk::Desync::jitterLeft, 0, 58, XORSTR("Desync jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_LEFT_MAX_4"), &Settings::AntiAim::SlowWalk::Desync::jitterLeftMax, 0, 58, XORSTR("Desync jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_4"), &Settings::AntiAim::SlowWalk::Desync::jitterRight, 1, 58, XORSTR("Desync jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##DESYNC_JITTER_RIGHT_MAX_4"), &Settings::AntiAim::SlowWalk::Desync::jitterRightMax, 1, 58, XORSTR("Desync jitter(right max): %0.f"));
                            break;
                        default: break;
                    }
                    switch(Settings::AntiAim::SlowWalk::Yaw::type)
                    {
                        case AntiAimYaw::JITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_4"), &Settings::AntiAim::SlowWalk::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_4"), &Settings::AntiAim::SlowWalk::Yaw::jitterRight, 0, 58, XORSTR("Yaw jitter(right): %0.f"));
                            break;
                        case AntiAimYaw::RANDOMJITTER:
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_4"), &Settings::AntiAim::SlowWalk::Yaw::jitterLeft, 0, 58, XORSTR("Yaw jitter(left min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_LEFT_MAX_4"), &Settings::AntiAim::SlowWalk::Yaw::jitterLeftMax, 0, 58, XORSTR("Yaw jitter(left max): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_4"), &Settings::AntiAim::SlowWalk::Yaw::jitterRight, 1, 58, XORSTR("Yaw jitter(right min): %0.f"));
                            ImGui::SliderInt(XORSTR("##YAW_JITTER_RIGHT_MAX_4"), &Settings::AntiAim::SlowWalk::Yaw::jitterRightMax, 1, 58, XORSTR("Yaw jitter(right max): %0.f"));
                            break;
                        case AntiAimYaw::SPIN:
                            ImGui::SliderInt(XORSTR("##YAW_SPINF_4"), &Settings::AntiAim::SlowWalk::Yaw::spinFactor, 0, 360, XORSTR("Yaw spinFactor: %0.f"));
                            break;
                        default: break;
                    }
                    ImGui::PopItemWidth();
                }
                ImGui::Separator();
                ImGui::Checkbox("Manual Direction", &Settings::AntiAim::Manual::enabled);
                ImGui::Columns(2);
                {
                    ImGui::Text("Left key");
                    ImGui::Spacing();
                    ImGui::Text("Back key");
                    ImGui::Spacing();
                    ImGui::Text("Right key");
                    ImGui::Spacing();
                    ImGui::Text("Desync/LBY invert key");
                }
                ImGui::NextColumn();
                {
                    UI::KeyBindButton(&Settings::AntiAim::Manual::left);
                    UI::KeyBindButton(&Settings::AntiAim::Manual::back);
                    UI::KeyBindButton(&Settings::AntiAim::Manual::right);
                    UI::KeyBindButton(&Settings::AntiAim::dFlipKey);
                }
                ImGui::EndColumns();
            }
            ImGui::EndChild();
        }
        ImGui::NextColumn();
        {
            ImGui::BeginChild(XORSTR("##AntiAim2"), ImVec2(0, 736), true);
            {
                ImGui::Separator();
                ImGui::Text(XORSTR("Features"));
                ImGui::Separator();

                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::Checkbox(XORSTR("QuickPeek"), &Settings::Ragebot::quickpeek::enabled);
                    ImGui::Checkbox(XORSTR("FakeDuck"), &Settings::FakeDuck::enabled);
                    ImGui::Checkbox(XORSTR("Angle Indicator"), &Settings::AngleIndicator::enabled);
                    ImGui::Checkbox(XORSTR("Animation Memes"), &Settings::AnimMemes::enabled);
                    ImGui::Checkbox(XORSTR("Choke OnShot"), &Settings::FakeLag::ChokeOnShot);
                }
                ImGui::NextColumn();
                {
                    UI::KeyBindButton(&Settings::Ragebot::quickpeek::key);
                    UI::KeyBindButton(&Settings::FakeDuck::key);
                }
                ImGui::EndColumns();
                ImGui::Separator();
                ImGui::Text(XORSTR("Fake Lag"));
                ImGui::Separator();
                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::Checkbox(XORSTR("Fake Lag"), &Settings::FakeLag::enabled);
                    ImGui::Checkbox(XORSTR("Adaptive Fake Lag"), &Settings::FakeLag::adaptive);
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 15, XORSTR("Amount: %0.f"));
                }
                ImGui::EndColumns();
                ImGui::Separator();
                ImGui::Text(XORSTR("Slow Walk"));
                ImGui::Separator();
                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::Checkbox(XORSTR("Slow Walk"), &Settings::SlowWalk::enabled);
                    ImGui::Combo(XORSTR("##SLOWTYPE"), (int *)&Settings::SlowWalk::mode, slowType, IM_ARRAYSIZE(slowType));
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    UI::KeyBindButton(&Settings::SlowWalk::key);
                    if (Settings::SlowWalk::mode == SlowMode::SAFETY)
                        ImGui::SliderFloat(XORSTR("##SLOWWALKSPEED"), &Settings::SlowWalk::speed, 0, 100, XORSTR("Amount: %0.f"));
                }
                ImGui::EndColumns();
                ImGui::Separator();
                ImGui::Text(XORSTR("Resolver"));
                ImGui::Separator();
                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::Checkbox(XORSTR("Resolver Override"), &Settings::Resolver::manual);
                }
                ImGui::NextColumn();
                {
                    if (Settings::Resolver::manual)
                    {
                        ImGui::Checkbox(XORSTR("Force Bruteforce"), &Settings::Resolver::forcebrute);
                    }
                }
                ImGui::EndColumns();
                ImGui::Columns(2, nullptr, false);
                {
                    if (!Settings::Resolver::forcebrute)
                    {
                        if (Settings::Resolver::manual)
                        {
                            ImGui::Text(XORSTR("Pitch"));
                        }
                    }
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    if (Settings::Resolver::manual)
                    {
                        if (!Settings::Resolver::forcebrute)
                        {
                            ImGui::SliderFloat(XORSTR("##GOALPITCH"), &Settings::Resolver::Pitch, -89, 89, XORSTR("Amount: %0.f"));
                        }
                    }
                }
                ImGui::EndColumns();
                ImGui::Columns(2, nullptr, false);
                {
                    if (!Settings::Resolver::forcebrute)
                    {
                        if (Settings::Resolver::manual)
                        {
                            ImGui::Text(XORSTR("Feet Yaw"));
                        }
                    }
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    if (Settings::Resolver::manual)
                    {
                        if (!Settings::Resolver::forcebrute)
                        {
                            ImGui::SliderFloat(XORSTR("##GOALFEETYAW"), &Settings::Resolver::goalFeetYaw, 0, 180, XORSTR("Amount: %0.f"));
                        }
                    }
                }
                ImGui::EndColumns();
                ImGui::Columns(2, nullptr, false);
                {
                    if (!Settings::Resolver::forcebrute)
                    {
                        if (Settings::Resolver::manual)
                        {
                            ImGui::Text(XORSTR("Eye Angles"));
                        }
                    }
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    if (Settings::Resolver::manual)
                    {
                        if (!Settings::Resolver::forcebrute)
                        {
                            ImGui::SliderFloat(XORSTR("##EYEANGLES"), &Settings::Resolver::EyeAngles, -60, 60, XORSTR("Amount: %0.f"));
                        }
                    }
                }
                ImGui::EndColumns();
            }
            ImGui::EndChild();
        }
        ImGui::EndColumns();
    }
    ImGui::EndGroup();
}