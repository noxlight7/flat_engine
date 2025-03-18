//
// Created by curo on 31.01.2025.
//

#ifndef OPENGL_TEXTURE_BUFFER_H
#define OPENGL_TEXTURE_BUFFER_H

#include <optional>
#include <unordered_set>
#include <glad/glad.h>
#include <vector>

#include "opengl_texture.hpp"
#include "texture_manager.hpp"

class OpenGLTextureManager final : public ITextureManager {
    std::vector<std::optional<OpenGLTexture>> m_textures;
    std::unordered_set<TextureID> m_deleted_textures;
    // GLuint current_unit = 0;

public:
    ~OpenGLTextureManager() override = default;
    OpenGLTextureManager() = default;
    OpenGLTextureManager(const OpenGLTextureManager &) = delete;
    TextureID loadTexture(const std::string &path) override;
    // void bindTexture(TextureID);
    void unbindAll() override;
    void freeTexture(TextureID) override;
    [[nodiscard]] ITexture* getTexture(TextureID texture_id) override;
};

#endif //OPENGL_TEXTURE_BUFFER_H
