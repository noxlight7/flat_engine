#pragma once
#include "display_objects.hpp"
#include "display_system.hpp"
#include "physics/base_physics.hpp"

using TerrainMap = std::unordered_map<TerrainID, Terrain>;
class IRendererWorld {
public:
	virtual ~IRendererWorld() = default;
	virtual void drawWorld(DisplaySystem& display_system,
		const DisplayObjects& object_types_textures,
		const TerrainMap& terrain_map,
		IRenderer* renderer) = 0;
	// virtual float getCameraHeight() = 0;
};
