// bool misc::double_tap(CUserCmd* m_pcmd)
// {
//         double_tap_enabled = true;

//         static auto recharge_double_tap = false;
//         static auto last_double_tap = 0;

//         if (recharge_double_tap)
//         {
//                 recharge_double_tap = false;
//                 recharging_double_tap = true;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;
//         }

//         if (recharging_double_tap)
//         {
//                 auto recharge_time = g_ctx.globals.weapon->can_double_tap() ? TIME_TO_TICKS(0.75f) : TIME_TO_TICKS(1.5f);

//                 if (!aim::get().should_stop && fabs(g_ctx.globals.fixed_tickbase - last_double_tap) > recharge_time)
//                 {
//                         last_double_tap = 0;

//                         recharging_double_tap = false;
//                         double_tap_key = true;
//                 }
//                 else if (m_pcmd->m_buttons & IN_ATTACK)
//                         last_double_tap = g_ctx.globals.fixed_tickbase;
//         }

//         if (!g_cfg.ragebot.enabled)
//         {
//                 double_tap_enabled = false;
//                 double_tap_key = false;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;
//         }

//         if (!g_cfg.ragebot.double_tap)
//         {
//                 double_tap_enabled = false;
//                 double_tap_key = false;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;
//         }

//         if (g_cfg.ragebot.double_tap_key.key <= KEY_NONE || g_cfg.ragebot.double_tap_key.key >= KEY_MAX)
//         {
//                 double_tap_enabled = false;
//                 double_tap_key = false;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;
//         }

//         if (double_tap_key && g_cfg.ragebot.double_tap_key.key != g_cfg.antiaim.hide_shots_key.key)
//                 hide_shots_key = false;

//         if (!double_tap_key)
//         {
//                 double_tap_enabled = false;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;

//        }

//         if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN) //-V807
//         {
//                 double_tap_enabled = false;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;
//         }

//         if (m_gamerules()->m_bIsValveDS())
//         {
//                 double_tap_enabled = false;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;
//         }

//         if (g_ctx.globals.fakeducking)
//         {
//                 double_tap_enabled = false;
//                 g_ctx.globals.ticks_allowed = 0;
//                 g_ctx.globals.next_tickbase_shift = 0;
//                 return false;
//         }

//         if (antiaim::get().freeze_check)
//                 return true;

//         auto max_tickbase_shift = g_ctx.globals.weapon->get_max_tickbase_shift();

//         if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && g_ctx.send_packet && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2 &>
//         {
//                 auto next_command_number = m_pcmd->m_command_number + 1;
//                 auto user_cmd = m_input()->GetUserCmd(next_command_number);

//                 memcpy(user_cmd, m_pcmd, sizeof(CUserCmd)); //-V598
//                 user_cmd->m_command_number = next_command_number;

//                 util::copy_command(user_cmd, max_tickbase_shift);

//                 if (g_ctx.globals.aimbot_working)
//                 {
//                         g_ctx.globals.double_tap_aim = true;
//                         g_ctx.globals.double_tap_aim_check = true;
//                 }

//                 recharge_double_tap = true;
//                 double_tap_enabled = false;
//                 double_tap_key = false;

//                 last_double_tap = g_ctx.globals.fixed_tickbase;
//         }
//         else if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
//                 g_ctx.globals.tickbase_shift = max_tickbase_shift;

//         return true;
// }
