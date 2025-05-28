//
// Created by curo on 28.05.2025.
//
#include "district_renderer.hpp"
#include "draw_info_factory.hpp"
#include "game_camera.hpp"


DistrictRenderer::DistrictRenderer(District* district)
    : IRendererWorld(), m_district(district) {
}

void DistrictRenderer::drawWorld(DisplaySystem& display_system,
    const DisplayObjects& object_types_textures, const std::unordered_map<TerrainID, TextureID>& terrain_map,
    IRenderer* renderer, uint64_t current_time, int screen_width, int screen_height) {
    auto& building_layout = display_system.getBuildingLayout();


    auto &old_camera = *dynamic_cast<Camera*>(renderer->getCurrentCamera().get());
    Camera camera(old_camera);

    auto& land_layer = building_layout[static_cast<int>(Layers::Land)];
    auto& terrain_layer = building_layout[static_cast<int>(Layers::Terrain)];
    auto render_area = camera.getVisibleRect(screen_width, screen_height);
    Position left_bottom = Position(render_area.m_left, render_area.m_bottom);
    Position right_top = Position(render_area.m_right, render_area.m_top);
    int x_start = std::max(0, left_bottom.m_index.x);
    int y_start = std::max(0, left_bottom.m_index.y);
    int x_end = std::min(right_top.m_index.x + 1, m_district->getCellsXAmount());
    int y_end = std::min(right_top.m_index.y + 1, m_district->getCellsYAmount());
    auto& terrain_matrix = m_district->getTerrainMatrix();
    for (int x_index = x_start; x_index < x_end; x_index++)
        for (int y_index = y_start; y_index < y_end; y_index++) {
            terrain_layer[x_index * m_district->getCellsYAmount() + y_index] = DrawInfoFactory::getTerrainDrawInfo(
                terrain_map.at(terrain_matrix(y_index, x_index)), x_index, y_index, camera);
            for (auto &draw_obj : m_district->getCell(x_index, y_index)->getInnerObjects()) {
                auto draw_info = DrawInfoFactory::getSpaceObjectDrawInfo(
                    *draw_obj, camera, object_types_textures);
                auto id = draw_obj->getID();
                land_layer[id] = draw_info;
                // auto info = draw_obj->getDrawInfo();
                // info->setOrigin(vec3(g_camera->getOrigin().getShift(
                // 	draw_obj->getPosition()), 0.0f));
                // info->draw();
            }
        }
    display_system.nextState(current_time);
}
