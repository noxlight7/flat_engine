//
// Created by curo on 31.01.2025.
//

#ifndef OPENGL_VERTEX_BUFFER_H
#define OPENGL_VERTEX_BUFFER_H
#include <vector>
#include <glad/glad.h>
#include <glm/vec2.hpp>

#include "opengl_graphics_constants.hpp"

struct Vertex {
    glm::vec2 position;
    glm::vec2 tex_coords;
};

class OpenGLVertexBuffer {
    GLuint m_vao = 0;       // vertex array object
    GLuint m_vbo = 0;       // vertex buffer object
    // GLuint m_ebo = 0;
    size_t m_capacity;      // максимальное число вершин
    size_t m_size = 0;      // число загруженных вершин
    // std::vector<uint32_t> m_indices;

public:
    explicit OpenGLVertexBuffer(size_t max_vertices = constants::k_vertex_buffer_capacity);
    ~OpenGLVertexBuffer();

    void uploadData(const std::vector<Vertex> &vertices);
    void bind() const;
    void unbind() const;
    void resize(size_t max_vertices);

    [[nodiscard]] size_t getUsagePercent() const;
};
#endif //OPENGL_VERTEX_BUFFER_H
