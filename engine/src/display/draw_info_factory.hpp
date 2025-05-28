//
// Created by curo on 18.03.2025.
//

#ifndef DRAW_INFO_FACTORY_H
#define DRAW_INFO_FACTORY_H
#include "game_camera.hpp"
#include "renderer.hpp"
#include "physics/physics.hpp"

class DrawInfoFactory {
public:
    static DrawInfo getLocatableObjectDrawInfo(LocatableObject &obj, const Camera &cam,
        const DisplayObjects& object_types_textures) {
        auto position = obj.getRenderOrigin() - cam.getGlobalOrigin();

        auto tex = object_types_textures[obj.getTypeID()];
        auto size = obj.getForm().getSize(0);

        return {
            .tex = tex,
            .position = {position.x, position.y},
            .size = size,
            .rotation = static_cast<float>(obj.getRotation() * 180 / math::g_pi)
        };
    }

    static DrawInfo getSpaceObjectDrawInfo(SpaceObject &obj, const Camera &cam,
        const DisplayObjects& object_types_textures) {
        return getLocatableObjectDrawInfo(obj, cam, object_types_textures);
    }

    static DrawInfo getTerrainDrawInfo(const TerrainID &texture_id, float x_index, float y_index, const Camera &cam) {
        auto position = Position(x_index, y_index).getRenderOrigin() - cam.getGlobalOrigin();
        return {
            .tex = texture_id,
            .position = {position.x + 0.5f, position.y + 0.5f},
            .size = {1.0f, 1.0f},
            .rotation = 0
        };
    }
};

#endif //DRAW_INFO_FACTORY_H
