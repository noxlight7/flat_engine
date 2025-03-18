//
// Created by curo on 01.02.2025.
//

#include <opengl/opengl_texture.hpp>
#include <glad/glad.h>
#include <iostream>

OpenGLTexture::OpenGLTexture(const GLuint gl_id, const int width, const int height, std::string name)
    : m_gl_id(gl_id), m_width(width), m_height(height), m_name(std::move(name)){}

OpenGLTexture::~OpenGLTexture() {
    if (m_gl_id)
        glDeleteTextures(1, &m_gl_id);
}



