#pragma once

#include "../ImGUI/imgui.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/vector.h"
#include "../SDK/IVModelRender.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include <vector>

namespace cbullet_tracer
{

	void log(IGameEvent* event);
	void render();
	class cbullet_tracer_info
	{
		public:
		cbullet_tracer_info(Vector src, Vector dst, float time, Color color)
		{
			this->src = src;
			this->dst = dst;
			this->time = time;
			this->color = color;
		}
		Vector src, dst;
		float time;
		Color color;
};
	std::vector<cbullet_tracer_info> logs;
};
