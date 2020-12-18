#include <vector>
#include <cstdint>
#include <thread>
#include "../settings.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../interfaces.h"
#include "../Utils/xorstring.h"
#include "../hooker.h"
#include "../Hooks/hooks.h"
#include "../Utils/bonemaps.h"
#include "../Utils/math.h"

namespace global {
    inline int tickcount;
    inline C_BasePlayer* local;
    void CreateMove(CUserCmd*);

}
