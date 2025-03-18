#pragma once
#include "display_objects.hpp"
#include "display_system.hpp"

class IRendererWorld {
public:
	virtual ~IRendererWorld() = default;
	virtual void drawWorld(DisplaySystem& display_system,
		const DisplayObjects& object_types_textures,
		IRenderer* renderer) = 0;
	// virtual float getCameraHeight() = 0;
};
