// findHudElement = relativeToAbsolute<decltype(findHudElement)>(reinterpret_cast<uintptr_t>(temp) + 5);
// void preserveKillfeed::run(bool roundStart)
// {
//     if (!config->misc.preserveKillfeed)
//         return;

//     static auto nextUpdate = 0.0f;

//     if (roundStart) {
//         nextUpdate = globalVars->curtime + 10.0f;
//         return;
//     }

//     if (nextUpdate > globalVars->curtime)
//         return;

//     nextUpdate = globalVars->realtime + 2.0f;

//     const auto deathNotice = findHudElement(hud, "CCSGO_HudDeathNotice");
//     if (!deathNotice)
//         return;

//     const auto deathNoticePanel = (*(UIPanel**)(*(deathNotice - 5 + 22) + 4));
//     const auto childPanelCount = deathNoticePanel->getChildCount();

//     for (int i = 0; i < childPanelCount; ++i) {
//         const auto child = deathNoticePanel->getChild(i);
//         if (!child)
//             continue;

//         if (child->hasClass("DeathNotice_Killer"))
//             child->setAttributeFloat("SpawnTime", globalVars->currenttime);
//     }
// }
