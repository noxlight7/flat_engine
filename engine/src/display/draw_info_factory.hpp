//
// Created by curo on 18.03.2025.
//

#ifndef DRAW_INFO_FACTORY_H
#define DRAW_INFO_FACTORY_H
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
            .size = size
        };
    }

    static DrawInfo getSpaceObjectDrawInfo(LocatableObject &obj, const Camera &cam,
        const DisplayObjects& object_types_textures) {
        return getLocatableObjectDrawInfo(obj, cam, object_types_textures);
    }
};

#endif //DRAW_INFO_FACTORY_H
