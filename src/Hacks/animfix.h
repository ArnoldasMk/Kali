#include "../interfaces.h"
#include <deque>

namespace AnimFix
{
       struct animation_info {
                animation_info(C_BasePlayer* player, std::deque<CCSGOAnimState*> animations)
                        : player(player), frames(std::move(animations)), last_spawn_time(0) { }

                void update_animations(CCSGOAnimState* to, CCSGOAnimState* from);

                C_BasePlayer* player{};
                std::deque<CCSGOAnimState*> frames;

                // last time this player spawned
                float last_spawn_time;
                float goal_feet_yaw;
                Vector last_reliable_angle;
        };

    void FrameStageNotify(ClientFrameStage_t stage);
}
