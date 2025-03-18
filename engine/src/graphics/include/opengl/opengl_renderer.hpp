//
// Created by curo on 30.01.2025.
//

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H
#include <renderer.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <opengl/opengl_texture_manager.hpp>
#include <opengl/opengl_vertex_buffer.hpp>

#include "opengl_shader.hpp"

class OpenGLRenderer final : public IRenderer {
    GLFWwindow* m_window = nullptr;
    glm::mat4 m_projection {};
    glm::mat4 m_view {};
    int m_width{}, m_height{};

    std::unique_ptr<OpenGLVertexBuffer> m_vertex_buffer;

    std::unique_ptr<OpenGLShader> m_shader;
    std::unique_ptr<OpenGLTextureManager> m_texture_manager;

public:
    bool init(void* window) override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    void clear(float r, float g, float b, float a) override;
    void drawLayer(const DrawLayer &previous, const DrawLayer& current, float dt_ratio, float camera_height) override;
    [[nodiscard]] ITextureManager* getTextureManager() override;

    void setViewPortSize(int width, int height) override;
    glm::vec2 getViewPortSize() override;

    void updateViewPortSize() override;

    void setCurrentCamera(std::shared_ptr<ICamera> camera) override;

    ~OpenGLRenderer() override;
};
#endif //OPENGL_RENDERER_H
