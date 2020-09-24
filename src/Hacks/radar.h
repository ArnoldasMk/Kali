#pragma once

namespace Radar
{
	enum EntityShape_t : int
	{
		SHAPE_CIRCLE,
		SHAPE_SQUARE,
		SHAPE_TRIANGLE,
		SHAPE_TRIANGLE_UPSIDEDOWN
	};

	void RenderWindow();

	//Hooks
	void BeginFrame();
};
