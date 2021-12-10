#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wformat-security"

#include "antiaimtab.h"

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Features/valvedscheck.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

void AntiAim::RenderMainMenu(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
    /*
    const char *pitchCustomType[] = {
        "Up",
        "Down",
        "Zero",
        "Fake Zero",
        "Fake Up",
        "Fake Down",
        "Fake Jitter",
    };
    */
    const char *aaState[] = {
        "Stand",
        "Air",
        "Move",
        "Slow Walk",
        "Fake Duck",
        "Lby Update",
    };
    const char *LegitAAType[] = {
        "OverWatchProof",
        "FakeLegitAA",
        "Experimental"};
    static char *slowType[] = {
        "Safety",
        "Accuracy",
    };
    const char *yType[] = {
        "NONE",
        "Static",
        "Balanced",
        "Jitter",
        "Brainfuck Fake Fake AA",
        "JitterRandom",
        "P100 SPIN",
    };
    const char *pitchType[] = {
        "Up",
        "Down",
        "Zero",
        "Emotion",
        "Fake Zero",
        "Fake Up",
        "Fake Down",
        "Fake Jitter",
    };

    const char *lbyType[] = {
        "Normal",
        "Opposite",
        "Sway",
    };
    const char *RageAntiAimType[] = {
        "Default Rage",
        "Free Stand",
    };
    /*
    else if (Settings::AntiAim::RageAntiAim::enabled == true)
    {
        Settings::AntiAim::LegitAntiAim::enabled == false;
        Settings::AntiAim::RageAntiAim::customaa::enabled == false; // -_-
        Settings::AntiAim::AADisabled::enabled == false;
    }
    else if (Settings::AntiAim::RageAntiAim::customaa::enabled == true)
    {
        Settings::AntiAim::LegitAntiAim::enabled == false;
        Settings::AntiAim::RageAntiAim::enabled == false;
        Settings::AntiAim::AADisabled::enabled == false;
    };
    */
    ImGui::SetCursorPos(ImVec2(185, 70));
    ImGui::BeginGroup();
    {
        ImGui::Columns(2, nullptr, false);
        {
            if (ImGui::Checkbox(XORSTR("Rage Anti Aim"), &Settings::AntiAim::RageAntiAim::enabled))
            {
                Settings::AntiAim::LegitAntiAim::enabled == false;  
                Settings::AntiAim::AADisabled::enabled == false;
            }
        }
        ImGui::NextColumn();
        {
            /* // SOON
            if (Settings::AntiAim::RageAntiAim::enabled == true)
            {
                ImGui::Checkbox(XORSTR("Custom Anti Aim"), &Settings::AntiAim::RageAntiAim::customaa::enabled);
            }
            */
        }
        ImGui::EndColumns();
    }
    ImGui::SetCursorPos(ImVec2(450, 70));
    ImGui::BeginGroup();
    {
        ImGui::Checkbox(XORSTR("Anti Aim Disabled"), &Settings::AntiAim::AADisabled::enabled);
    }
    ImGui::EndGroup();
    ImGui::SetCursorPos(ImVec2(180, 100));
    ImGui::BeginGroup();
    {
        ImGui::Columns(2, nullptr, false);
        {
            ImGui::BeginChild(XORSTR("##AntiAim1"), ImVec2(0, 730), true);
            {
                if (Settings::AntiAim::RageAntiAim::enabled == false)
                {
                    ImGui::Separator();
                    ImGui::Text(XORSTR("Legit Anti Aim"));
                    ImGui::Separator();
                    ImGui::Columns(1);
                    {
                        ImGui::Checkbox(XORSTR("Enabled"), &Settings::AntiAim::LegitAntiAim::enabled);
                        Settings::AntiAim::RageAntiAim::enabled == false;
                        Settings::AntiAim::RageAntiAim::customaa::enabled == false; // -_-
                        Settings::AntiAim::AADisabled::enabled == false;
                    }
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Anti Aim Type"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##LegitAAType"), (int *)&Settings::AntiAim::LegitAntiAim::legitAAtype, LegitAAType, IM_ARRAYSIZE(LegitAAType));
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("InvertKey"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        UI::KeyBindButton(&Settings::AntiAim::LegitAntiAim::InvertKey);
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Yaw Desync Angle"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat(XORSTR("##mEES"), &Settings::AntiAim::offsat, -180, 180, "%.0f");
                    }
                    ImGui::EndColumns();
                    ImGui::Separator();
                    ImGui::Text(XORSTR("Features"));
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Checkbox(XORSTR("Legit freestand"), &Settings::AntiAim::AutoInvert);
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::Checkbox(XORSTR("Set arms to fake side"), &Settings::AntiAim::arms);
                    }
                    ImGui::EndColumns();
                }
                else if (Settings::AntiAim::RageAntiAim::enabled == true, Settings::AntiAim::RageAntiAim::customaa::enabled == false)
                {

                    ImGui::Separator();
                    ImGui::Text(XORSTR("Rage Anti Aim"));
                    ImGui::Separator();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Anti Aim Type"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##RageAntiAimType"), (int *)&Settings::AntiAim::RageAntiAim::Type, RageAntiAimType, IM_ARRAYSIZE(RageAntiAimType));
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Yaw"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##YACTUALTYPE"), (int *)&Settings::AntiAim::Yaw::typeReal, yType, IM_ARRAYSIZE(yType));
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Pitch"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##PITCHTYPE"), (int *)&Settings::AntiAim::pitchtype, pitchType, IM_ARRAYSIZE(pitchType));
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        if (Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Randome || Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::JitterSwitch || Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::JitterRandom || Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Spin)
                            ImGui::Text(XORSTR("Jitter Amount (Percent)"));
                        if (Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Static)
                            ImGui::Text(XORSTR("Static Offset"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        if (Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Randome || Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::JitterSwitch || Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::JitterRandom || Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Spin)
                            ImGui::SliderFloat(XORSTR("##RealJitterPercentage"), &Settings::AntiAim::RageAntiAim::JitterPercent, 1, 100, "%.0f ");
                        if (Settings::AntiAim::Yaw::typeReal == AntiAimRealType_Y::Static)
                            ImGui::SliderFloat(XORSTR("##StaticPercentage"), &Settings::AntiAim::RageAntiAim::AntiAimOffset, 1, 360, "%.0f");
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("Body Lean Amount"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat(XORSTR("##BodyLeanAmmount"), &Settings::AntiAim::RageAntiAim::bodylean, 1, 100, "%.0f"); // I think it's not working tho, idk
                        ToolTip::Show(XORSTR("This feature might be broken, use carefully"), ImGui::IsItemHovered());
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text(XORSTR("InvertKey"));
                    }
                    ImGui::NextColumn();
                    {
                        UI::KeyBindButton(&Settings::AntiAim::RageAntiAim::InvertKey);
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(1);
                    {
                        ImGui::Checkbox(XORSTR("At Targets (alpha)"), &Settings::AntiAim::RageAntiAim::atTheTarget);
                        ImGui::Checkbox(XORSTR("Invert on hurt"), &Settings::AntiAim::RageAntiAim::invertOnHurt);
                        ImGui::Checkbox(XORSTR("Legit AA on E"), &Settings::AntiAim::RageAntiAim::legitkey::enabled);
                    }
                    ImGui::Separator();
                    ImGui::Text(XORSTR("Manual Anti Aim"));
                    ImGui::Separator();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Checkbox(XORSTR("Enabled"), &Settings::AntiAim::ManualAntiAim::Enabled);
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::Checkbox(XORSTR("Auto Direction"), &Settings::AntiAim::AutoInvert);
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text("Align Right");
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::RightButton);
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text("Align Back");
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::backButton);
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Text("Align Left");
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::LeftButton);
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Separator();
                    ImGui::Text(XORSTR("Edging(Alfa)"));
                    ImGui::Separator();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Checkbox(XORSTR("Enabled"), &Settings::AntiAim::HeadEdge::enabled);
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::SliderFloat(XORSTR("##EDGEDISTANCE"), &Settings::AntiAim::HeadEdge::distance, 20, 30, "Distance: %0.0f");
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Separator();
                    ImGui::Text(XORSTR("LBY"));
                    ImGui::Separator();
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::Checkbox(XORSTR("Break LBY"), &Settings::AntiAim::RageAntiAim::lby::enabled);
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##LbyType"), (int *)&Settings::AntiAim::RageAntiAim::lbym, lbyType, IM_ARRAYSIZE(lbyType));
                    }
                    ImGui::EndColumns();
                }
                /* SOON
                else if (Settings::AntiAim::RageAntiAim::customaa::enabled == true, Settings::AntiAim::RageAntiAim::enabled == true)
                {
                    ImGui::Separator();
                    ImGui::Text(XORSTR("Custom Anti Aim"));
                    ImGui::Separator();
                    ImGui::SliderInt(XORSTR("##YAWANGLE"), &Settings::AntiAim::RageAntiAim::customaa::standang, -180, 180, "Yaw Desync Angle : %.0f");
                    ImGui::SliderInt(XORSTR("##YAWAIRANGLE"), &Settings::AntiAim::RageAntiAim::customaa::airang, -180, 180, "Yaw Desync Angle : %.0f");
                    ImGui::Columns(2, nullptr, false);
                    {
                        ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
                        ImGui::Text(XORSTR("Pitch"));
                    }
                    ImGui::NextColumn();
                    {
                        ImGui::PushItemWidth(-1);
                        ImGui::Combo(XORSTR("##PITCHTYPE"), (int *)&Settings::AntiAim::pitchtype, pitchType, IM_ARRAYSIZE(pitchType));
                        ImGui::PopItemWidth();
                    }
                    ImGui::EndColumns();
                    ImGui::Checkbox(XORSTR("Randomize Yaw"), &Settings::AntiAim::RageAntiAim::customaa::sidemove);
                    ImGui::Checkbox(XORSTR("Micromovements"), &Settings::AntiAim::RageAntiAim::customaa::sidemove);
                }
                */
            }
            ImGui::EndChild();
        }
        ImGui::NextColumn();
        {
            ImGui::BeginChild(XORSTR("##AntiAim2"), ImVec2(0, 730), true);
            {
                ImGui::Separator();
                ImGui::Text(XORSTR("Features"));
                ImGui::Separator();

                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::Checkbox(XORSTR("QuickPeek"), &Settings::Ragebot::quickpeek::enabled);
                    ImGui::Checkbox(XORSTR("FakeDuck"), &Settings::AntiAim::FakeDuck::enabled);
                    ImGui::Checkbox(XORSTR("Fake Peek"), &Settings::AntiAim::RageAntiAim::fakepeek);

                    ImGui::Checkbox(XORSTR("Angle Indicator"), &Settings::AngleIndicator::enabled);
                    ImGui::Checkbox(XORSTR("Spin on air"), &Settings::AntiAim::airspin::enabled);
                    ImGui::Checkbox(XORSTR("Animation Memes"), &Settings::AnimMemes::enabled);
                    ImGui::Checkbox(XORSTR("Choke OnShot"), &Settings::AntiAim::ChokeOnShot);
                }
                ImGui::NextColumn();
                {
                    UI::KeyBindButton(&Settings::Ragebot::quickpeek::key);
                    UI::KeyBindButton(&Settings::AntiAim::FakeDuck::fakeDuckKey);
                    UI::KeyBindButton(&Settings::AntiAim::RageAntiAim::fakeheadkey);
                }
                ImGui::EndColumns();
                ImGui::Separator();
                ImGui::Text(XORSTR("Fake Lag"));
                ImGui::Separator();
                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::Checkbox(XORSTR("Fake Lag"), &Settings::FakeLag::enabled);
                    ImGui::Checkbox(XORSTR("Fake Lag Mic Key"), &Settings::FakeLag::microphone);
                    ImGui::Checkbox(XORSTR("Random Fake Lag"), &Settings::AntiAim::randomLag::enabled);
                    ImGui::Checkbox(XORSTR("Adaptive Fake Lag"), &Settings::FakeLag::adaptive);
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 15, XORSTR("Amount: %0.f"));
                    UI::KeyBindButton(&Settings::FakeLag::microphoneKey);
                    UI::KeyBindButton(&Settings::FakeLag::ckey);
                }
                ImGui::EndColumns();
                ImGui::Separator();
                ImGui::Text(XORSTR("Slow Walk"));
                ImGui::Separator();
                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::Checkbox(XORSTR("Slow Walk"), &Settings::AntiAim::SlowWalk::enabled);
                    ImGui::Combo(XORSTR("##SLOWTYPE"), (int *)&Settings::AntiAim::SlowWalk::mode, slowType, IM_ARRAYSIZE(slowType));
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    UI::KeyBindButton(&Settings::AntiAim::SlowWalk::key);
                    if (Settings::AntiAim::SlowWalk::mode == SlowMode::SAFETY)
                        ImGui::SliderFloat(XORSTR("##SlowWalkSPeed"), &Settings::AntiAim::SlowWalk::Speed, 0, 100, XORSTR("Amount: %0.f"));
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