//
// Created by curo on 01.02.2025.
//

#include <opengl/opengl_vertex_buffer.hpp>
#include <stdexcept>

OpenGLVertexBuffer::OpenGLVertexBuffer(const size_t max_vertices)
    : m_capacity(max_vertices) {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m_capacity * sizeof(Vertex)),
        nullptr,
        GL_DYNAMIC_DRAW
        );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*) offsetof(Vertex, position)
        );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*) offsetof(Vertex, tex_coords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void OpenGLVertexBuffer::uploadData(const std::vector<Vertex>& vertices) {
    if (vertices.size() > m_capacity) {
        throw std::runtime_error("Vertex buffer overflow");
    }

    m_size = vertices.size();

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data()
        );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLVertexBuffer::bind() const {
    glBindVertexArray(m_vao);
}

void OpenGLVertexBuffer::unbind() const {
    glBindVertexArray(0);
}

void OpenGLVertexBuffer::resize(const size_t max_vertices) {
    m_capacity = max_vertices;
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m_capacity * sizeof(Vertex)),
        nullptr,
        GL_DYNAMIC_DRAW
        );
}

size_t OpenGLVertexBuffer::getUsagePercent() const {
    return (m_size * 100) / m_capacity;
}
