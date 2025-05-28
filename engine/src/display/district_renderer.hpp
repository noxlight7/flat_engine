#pragma once

#include "renderer_world.hpp"
#include "physics/district_net.hpp"

class DistrictRenderer : public IRendererWorld {
public:
    DistrictRenderer(District* district);
    void drawWorld(
        DisplaySystem& display_system,
        const DisplayObjects& object_types_textures,
        const std::unordered_map<TerrainID, TextureID>& terrain_map,
        IRenderer* renderer,
        uint64_t current_time,
        int screen_width,
        int screen_height) override;


private:
    District* m_district;
};
