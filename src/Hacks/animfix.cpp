// include "animfix.h"

// // #include "../Memory.h"
// // #include "../MemAlloc.h"
// // #include "../Interfaces.h"

// // #include "../SDK/LocalPlayer.h"
// // #include "../SDK/Cvar.h"
// // #include "../SDK/GlobalVars.h"
// // #include "../SDK/Entity.h"
// // #include "../SDK/UserCmd.h"
// // #include "../SDK/ConVar.h"
// // #include "../SDK/Input.h"

// Animations::Datas Animations::data;

// void Animations::update(UserCmd* cmd, bool& sendPacket) noexcept
// {
// C_BasePlayer *localPlayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
//     if (!localPlayer || !localPlayer->GetAlive())
//         return;
//     data.viewangles = cmd->viewangles;
//     data.sendPacket = sendPacket;
// }

// void Animations::fake() noexcept
// {
// }

// void Animations::real() noexcept
// {
// }

// void Animations::players() noexcept
// {
// C_BasePlayer *localPlayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

//     if (!localPlayer)
//         return;

//     for (int i = 1; i <= engine->GetMaxClients(); i++)
//     {
//                         C_BasePlayer *entity = (C_BasePlayer *)entityList->GetClientEntity(i);

//         if (!entity || entity == localPlayer || entity->GetDormant() || !entity->GetAlive() || !entity->GetAnimState())
//             continue;

//         if (!data.player[i].once)
//         {
//             data.player[i].poses = entity->pose_parameters();
//             data.player[i].abs = entity->GetAnimState()->m_flGoalFeetYaw;
//             data.player[i].simtime = 0;
//             data.player[i].once = true;
//         }

//         if(data.player[i].networked_layers.empty())
//             std::memcpy(&data.player[i].networked_layers, entity->GetAnimOverlay(), sizeof(AnimationLayer) * entity->getAnimationLayerCount());

//         while (entity->GetAnimState()->m_iLastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
//             entity->GetAnimState()->m_iLastClientSideAnimationUpdateFramecount -= 1;
//         entity->InvalidateBoneCache();
//         memory->setAbsOrigin(entity, entity->origin());
//         *entity->getEffects() &= ~0x1000;
//         *entity->getAbsVelocity() = entity->velocity();
//         std::memcpy(&data.player[i].networked_layers, entity->GetAnimOverlay(), sizeof(AnimationLayer) * entity->getAnimationLayerCount());
//         entity->ClientSideAnimation() = true;
//  /*       if (entity->isOtherEnemy(localPlayer.get()) && config->ragebotExtra.resolver && data.player[i].chokedPackets >= 1 && localPlayer->GetAlive())
//             entity->GetAnimState()->GoalFeetYaw = Resolver::CalculateFeet(entity);*/
//         entity->UpdateClientSideAnimation();
//         entity->ClientSideAnimation() = false;
//         if (data.player[i].simtime != entity->simulationTime())
//         {
//             data.player[i].chokedPackets = static_cast<int>((entity->simulationTime() - data.player[i].simtime) / memory->globalVars->intervalPerTick) - 1;
//             data.player[i].simtime = entity->simulationTime();
//             data.player[i].poses = entity->pose_parameters();
//             data.player[i].abs = entity->GetAnimState()->m_flGoalFeetYaw;
//         }
//         entity->GetAnimState()->m_flFeetYawRate = 0.f;
//         memory->setAbsAngle(entity, Vector{ 0,data.player[i].abs,0 });
//         data.player[i].networked_layers[12].weight = std::numeric_limits<float>::epsilon();
//         std::memcpy(entity->GetAnimOverlay(), &data.player[i].networked_layers, sizeof(AnimationLayer) * entity->getAnimationLayerCount());
//         entity->pose_parameters() = data.player[i].poses;
//         //entity->InvalidateBoneCache();
//         entity->setupBones(data.player[i].matrix, 256, 0x7FF00, memory->globalVars->currenttime);
//         auto backup = data.lastest[i];
//         auto boneCache = *(int**)(entity + 0x2910);
//         auto countBones = *(int*)(entity + 0x291C);

//         backup.boneCache = boneCache;
//         backup.countBones = countBones;
//         backup.mins = entity->getCollideable()->obbMins();
//         backup.max = entity->getCollideable()->obbMaxs();
//         backup.origin = entity->getAbsOrigin();
//     }
// }

// void Animations::setup(Entity* entity, Backtrack::Record record) noexcept
// {
// C_BasePlayer *localPlayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

//     if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->GetAlive())
//         return;
//     auto backup = data.backup[entity->index()];
//     auto boneCache = *(int**)(entity + 0x2910);
//     auto countBones = *(int*)(entity + 0x291C);

//     backup.boneCache = boneCache;
//     backup.countBones = countBones;
//     backup.mins = entity->getCollideable()->obbMins();
//     backup.max = entity->getCollideable()->obbMaxs();
//     backup.origin = entity->getAbsOrigin();
//     entity->InvalidateBoneCache();
//     memcpy(boneCache, record.matrix, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));

//     entity->getCollideable()->obbMins() = record.mins;
//     entity->getCollideable()->obbMaxs() = record.max;
//     memory->setAbsOrigin(entity, record.origin);
//     backup.hasBackup = true;
// }

// void Animations::finishSetup(Entity* entity) noexcept
// {
// C_BasePlayer *localPlayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

//     if (!entity || entity == localPlayer || entity->GetDormant() || !entity->GetAlive())
//         return;

//     auto backup = data.lastest[entity->index()];
//     if (data.backup[entity->index()].hasBackup)
//     {
//         auto boneCache = *(int**)(entity + 0x2910);
//         entity->InvalidateBoneCache();
//         memcpy(boneCache, backup.boneCache, sizeof(matrix3x4) * std::clamp(backup.countBones, 0, 256));

//         entity->getCollideable()->obbMins() = backup.mins;
//         entity->getCollideable()->obbMaxs() = backup.max;

//         memory->setAbsOrigin(entity, backup.origin);
//         data.backup[entity->index()].hasBackup = false;
//     }
//     else if (data.backupResolver[entity->index()].hasBackup)
//     {
//         auto boneCache = *(int**)(entity + 0x2910);
//         entity->InvalidateBoneCache();
//         memcpy(boneCache, backup.boneCache, sizeof(matrix3x4) * std::clamp(backup.countBones, 0, 256));

//         entity->getCollideable()->obbMins() = backup.mins;
//         entity->getCollideable()->obbMaxs() = backup.max;

//         memory->setAbsOrigin(entity, backup.origin);
//         data.backupResolver[entity->index()].hasBackup = false;
//     }
// } 
