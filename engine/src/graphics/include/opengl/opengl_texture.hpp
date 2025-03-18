//
// Created by curo on 01.02.2025.
//

#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H
#include <texture.hpp>
#include <string>
#include <glad/glad.h>

class OpenGLTexture final: public ITexture {
    GLuint m_gl_id = 0;
    int m_width = 0;
    int m_height = 0;
    std::string m_name;

public:
    OpenGLTexture(GLuint gl_id, int width, int height, std::string name);
    ~OpenGLTexture() override;

    OpenGLTexture(const OpenGLTexture&) = delete;
    OpenGLTexture& operator=(const OpenGLTexture&) = delete;

    // Реализуем конструктор перемещения:
    OpenGLTexture(OpenGLTexture&& other) noexcept
        : m_gl_id(other.m_gl_id), m_width(other.m_width), m_height(other.m_height), m_name(std::move(other.m_name)) {
        other.m_gl_id = 0;
    }

    OpenGLTexture& operator=(OpenGLTexture&& other) noexcept {
        if (this != &other) {
            // Освобождаем собственный ресурс
            if (m_gl_id != 0) {
                glDeleteTextures(1, &m_gl_id);
            }
            m_gl_id = other.m_gl_id;
            m_width = other.m_width;
            m_height = other.m_height;
            m_name = std::move(other.m_name);
            other.m_gl_id = 0;
        }
        return *this;
    }

    [[nodiscard]] int getWidth() const override {return m_width;}
    [[nodiscard]] int getHeight() const override {return m_height;}
    [[nodiscard]] std::string getName() const override {return m_name;}
    [[nodiscard]] GLuint getGlID() const {return m_gl_id;}
};
#endif //OPENGL_TEXTURE_H
