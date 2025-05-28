#pragma once
#include "display_objects.hpp"
#include "display_system.hpp"
#include "physics/base_physics.hpp"

class IRendererWorld {
public:
	virtual ~IRendererWorld() = default;
	virtual void drawWorld(DisplaySystem& display_system,
		const DisplayObjects& object_types_textures,
		const std::unordered_map<TerrainID, TextureID>& terrain_map,
		IRenderer* renderer,
		uint64_t current_time,
		int screen_width,
		int screen_height) = 0;

};
