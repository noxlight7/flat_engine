//
// Created by curo on 31.01.2025.
//

#include "opengl/opengl_texture_manager.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>
#include <glad/glad.h>

TextureID OpenGLTextureManager::loadTexture(const std::string &path) {
    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        throw std::runtime_error("[OpenGLTextureManager]: Failed to load image " + path);
    }

    const GLint format = channels == 4 ? GL_RGBA : GL_RGB;

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Добавляем проверку размеров текстуры
    GLint texWidth = 0, texHeight = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
    std::cout << "Loaded texture: " << path << " with size " << texWidth << "x" << texHeight << std::endl;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);


    if (m_deleted_textures.empty()) {
        m_textures.emplace_back(OpenGLTexture(textureID, width, height, path));
        return m_textures.size() - 1;
    } else {
        TextureID buffer_texture_id = m_deleted_textures.extract(m_deleted_textures.begin()).value();
        m_textures[buffer_texture_id] = OpenGLTexture(textureID, width, height, path);
        return buffer_texture_id;
    }
}

// void OpenGLTextureManager::bindTexture(TextureID) {
//     glActiveTexture(GL_TEXTURE0 + current_unit);
//     glBindTexture(GL_TEXTURE_2D, m_textures[current_unit]->getGlID());
//     current_unit++;
// }

void OpenGLTextureManager::unbindAll() {
    for (GLuint i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


void OpenGLTextureManager::freeTexture(const TextureID texture_id) {
    if (texture_id >= m_textures.size() || !m_textures[texture_id].has_value()) {
        return;
    }

    m_textures[texture_id].reset();
    m_deleted_textures.insert(texture_id);
}

ITexture* OpenGLTextureManager::getTexture(const TextureID texture_id) {
    if (texture_id < m_textures.size() and m_textures[texture_id].has_value()) {
        return &m_textures[texture_id].value();
    }
    return nullptr;
}
