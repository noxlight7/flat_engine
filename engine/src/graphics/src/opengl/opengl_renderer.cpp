//
// Created by curo on 30.01.2025.
//
#include <iostream>
#include <opengl/opengl_renderer.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opengl/opengl_vertex_buffer.hpp>
#include "res/opengl/embedded_shaders.hpp"

bool OpenGLRenderer::init(void* window) {
    m_window = static_cast<GLFWwindow*>(window);
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    std::cout << "OpenGL Renderer " << glGetString(GL_VERSION) << std::endl;

    updateViewPortSize();

    m_shader = std::make_unique<OpenGLShader>(
        k_sprite_vert,
        k_sprite_frag);

    m_vertex_buffer = std::make_unique<OpenGLVertexBuffer>();
    m_texture_manager = std::make_unique<OpenGLTextureManager>();
    m_view = glm::mat4(1.0f);

    return true;
}

void OpenGLRenderer::shutdown() {
    m_texture_manager = nullptr;
    m_shader = nullptr;
    m_vertex_buffer = nullptr;
}

void OpenGLRenderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::endFrame() {
    glfwSwapBuffers(m_window);
}

void OpenGLRenderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void OpenGLRenderer::drawLayer(
    const DrawLayer &previous, const DrawLayer& current,
    float dt_ratio, float camera_height
    ) {
    if (current.empty()) return;

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    // std::cout << "width = " << width << ", height = " << height << std::endl;

    // Группируем объекты по их TextureID
    std::unordered_map<TextureID, std::vector<DrawInfo>> groups;
    for (const auto& [key, draw] : current) {
        if (auto it = previous.find(key); it != previous.end()) {
            const DrawInfo& prev_draw = it->second;
            DrawInfo draw_info {
                .tex = draw.tex,
                .position = prev_draw.position + (draw.position - prev_draw.position) * dt_ratio,
                .size = prev_draw.size + (draw.size - prev_draw.size) * dt_ratio
            };
            groups[draw.tex].push_back(draw_info);
        } else {
            groups[draw.tex].push_back(draw);
        }
    }

    // Для каждой группы объектов с одинаковой текстурой
    for (const auto& groupPair : groups) {
        TextureID texId = groupPair.first;
        const auto& objects = groupPair.second;

        // Получаем текстуру из менеджера (предполагается, что ITexture* можно привести к OpenGLTexture*)
        ITexture* tex = getTextureManager()->getTexture(texId);
        if (!tex) continue;  // Если текстура не найдена — пропускаем

        const auto* oglTex = dynamic_cast<OpenGLTexture*>(tex);
        // Активируем текстурный юнит 0 и привязываем текстуру
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, oglTex->getGlID());
        // std::cout << "Binding texture (ID = " << oglTex->getGlID() << ") for group with TextureID " << texId << std::endl;
        if (!glIsTexture(oglTex->getGlID())) {
            std::cerr << "Ошибка: идентификатор текстуры " << oglTex->getGlID() << " не распознан как корректный объект текстуры." << std::endl;
        }
        // Проверка ошибок OpenGL
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error after glBindTexture: " << error << std::endl;
        }
        // Собираем вершины для всех объектов в группе
        std::vector<Vertex> vertices;
        vertices.reserve(objects.size() * 6);
        for (const auto& draw : objects) {
            // Предполагаем, что draw.position — позиция (верхний левый угол) и draw.size — размеры в пикселях.
            const glm::vec2 pos = draw.position;
            const glm::vec2 size = draw.size;

            // Первый треугольник
            vertices.push_back({ glm::vec2(pos.x, pos.y),              glm::vec2(0.0f, 0.0f) });
            vertices.push_back({ glm::vec2(pos.x + size.x, pos.y),       glm::vec2(1.0f, 0.0f) });
            vertices.push_back({ glm::vec2(pos.x + size.x, pos.y + size.y),glm::vec2(1.0f, 1.0f) });
            // Второй треугольник
            vertices.push_back({ pos,                                 glm::vec2(0.0f, 0.0f) });
            vertices.push_back({ glm::vec2(pos.x + size.x, pos.y + size.y),glm::vec2(1.0f, 1.0f) });
            vertices.push_back({ glm::vec2(pos.x, pos.y + size.y),       glm::vec2(0.0f, 1.0f) });
        }

        // Загружаем данные вершин в vertex buffer
        m_vertex_buffer->uploadData(vertices);
        m_vertex_buffer->bind();
        m_shader->use();

        // Передаём матрицы в шейдер
        m_shader->setMat4("u_projection", m_projection);
        m_shader->setMat4("u_view", m_view);
        // Шейдер ожидает текстуру на юните 0
        m_shader->setInt("u_texture", 0);

        // Отрисовываем группу одним вызовом glDrawArrays
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        m_vertex_buffer->unbind();
    }
}

ITextureManager* OpenGLRenderer::getTextureManager() {
    return m_texture_manager.get();
}

void OpenGLRenderer::setViewPortSize(int width, int height) {
    m_width = width;
    m_height = height;
    setCurrentCamera(getCurrentCamera());
    glViewport(0, 0, width, height);
}

glm::vec2 OpenGLRenderer::getViewPortSize() {
    return {m_width, m_height};
}

void OpenGLRenderer::updateViewPortSize() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    setViewPortSize(width, height);
}

void OpenGLRenderer::setCurrentCamera(std::shared_ptr<ICamera> camera) {
    IRenderer::setCurrentCamera(camera);
    if (auto c_camera = getCurrentCamera()) {
        float aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
        float camera_half_height = c_camera->getHeight() / 2.0f;
        m_projection = glm::ortho(
            -aspect * camera_half_height,  // left
             aspect * camera_half_height,  // right
             -camera_half_height,           // bottom
             camera_half_height,           // top
            -1.0f,
             1.0f
        );
    }
}

OpenGLRenderer::~OpenGLRenderer() = default;
