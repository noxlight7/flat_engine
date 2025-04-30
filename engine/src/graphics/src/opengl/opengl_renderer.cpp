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

struct InstanceData {
    glm::vec2 pos;
    glm::vec2 size;
    float rotation;
};

std::vector<Vertex> quadVertices = {
    // Первый треугольник
    { {-0.5f, -0.5f}, {0.0f, 0.0f} },
    { { 0.5f, -0.5f}, {1.0f, 0.0f} },
    { { 0.5f,  0.5f}, {1.0f, 1.0f} },
    // Второй треугольник
    { {-0.5f, -0.5f}, {0.0f, 0.0f} },
    { { 0.5f,  0.5f}, {1.0f, 1.0f} },
    { {-0.5f,  0.5f}, {0.0f, 1.0f} }
};

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
                .size = prev_draw.size + (draw.size - prev_draw.size) * dt_ratio,
                .rotation = prev_draw.rotation + (draw.rotation - prev_draw.rotation) * dt_ratio
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

    // Получаем текстуру
    ITexture* tex = getTextureManager()->getTexture(texId);
    if (!tex) continue;
    const auto* oglTex = dynamic_cast<OpenGLTexture*>(tex);
    if (!oglTex) continue;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, oglTex->getGlID());

    // Собираем инстанс-данные для каждого объекта.
    std::vector<InstanceData> instances;
    instances.reserve(objects.size());
    for (const auto& draw : objects) {
        InstanceData inst;
        // Если требуется интерполяция между предыдущим и текущим состоянием, то делается как ранее:
        // inst.pos = prev_draw.position + (draw.position - prev_draw.position) * dt_ratio;
        // inst.size = prev_draw.size + (draw.size - prev_draw.size) * dt_ratio;
        // inst.rotation = prev_draw.rotation + (draw.rotation - prev_draw.rotation) * dt_ratio;
        // В простейшем случае:
        inst.pos = draw.position;
        inst.size = draw.size;
        inst.rotation = draw.rotation;
        instances.push_back(inst);
    }

    // Создаем VAO и VBO для квадрата и инстанс-данных.
    GLuint VAO, quadVBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    // Загрузка вершин квадрата (не инстансированные данные)
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(Vertex), quadVertices.data(), GL_STATIC_DRAW);
    // Атрибут 0: позиция
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // Атрибут 1: текстурные координаты
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

    // Загрузка инстанс-данных
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData), instances.data(), GL_DYNAMIC_DRAW);

    // Атрибут 2: instancePos (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, pos));
    glVertexAttribDivisor(2, 1);  // Использовать одно и то же значение для каждого экземпляра

    // Атрибут 3: instanceSize (vec2)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, size));
    glVertexAttribDivisor(3, 1);

    // Атрибут 4: instanceRotation (float)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, rotation));
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Отправляем данные в шейдер
    m_shader->use();
    m_shader->setMat4("u_projection", m_projection);
    m_shader->setMat4("u_view", m_view);
    m_shader->setInt("u_texture", 0);

    // Отрисовка с использованием инстансинга
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(instances.size()));
    glBindVertexArray(0);

    // Освобождаем созданные буферы (в реальном приложении их можно кешировать)
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteVertexArrays(1, &VAO);
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
