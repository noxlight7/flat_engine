//
// Created by curo on 01.02.2025.
//

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <string>
#include <vector>
#include <cstdint>

#include "texture.hpp"
using TextureID = uint16_t;

class ITextureManager {
public:
    virtual ~ITextureManager() = default;

    virtual TextureID loadTexture(const std::string &path) = 0;
    virtual void freeTexture(TextureID texture_id) = 0;
    virtual void unbindAll() = 0;
    [[nodiscard]] virtual ITexture* getTexture(TextureID texture_id) = 0;
};

#endif //TEXTURE_MANAGER_H
