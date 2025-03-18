//
// Created by curo on 30.01.2025.
//

#ifndef RENDERER_H
#define RENDERER_H
#include <cstdint>
#include <memory>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <vector>
#include <glm/vec2.hpp>

#include "camera.hpp"
#include "texture_manager.hpp"

enum class RendererType { OPEN_GL, VULKAN, METAL };

struct DrawInfo {
    TextureID tex;
    glm::vec2 position;
    glm::vec2 size;
};

using DrawLayer = std::unordered_map<uint32_t, DrawInfo>;

using DrawLayout = std::vector<DrawLayer>;


class IRenderer {
    std::shared_ptr<ICamera> m_current_camera;
public:
    virtual ~IRenderer() = default;

    virtual bool init(void *window) = 0;
    virtual void shutdown() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear(float r, float g, float b, float a) = 0;
    virtual void drawLayer(
        const DrawLayer &previous,
        const DrawLayer& current,
        float dt_ratio,
        float camera_height
        ) = 0;

    void drawLayout(
        const DrawLayout &previous,
        const DrawLayout &current,
        float dt_ratio,
        float camera_height) {
        for (const auto [
            previous_layer,
            current_layer] :
                std::views::zip(previous, current)) {
            drawLayer(previous_layer, current_layer, dt_ratio, camera_height);
        }
    }

    static std::unique_ptr<IRenderer> createRenderer(RendererType type);

    [[nodiscard]] virtual ITextureManager* getTextureManager() = 0;

    virtual void setViewPortSize(int width, int height) = 0;
    virtual void updateViewPortSize() = 0;
    virtual glm::vec2 getViewPortSize() = 0;

    virtual void setCurrentCamera(std::shared_ptr<ICamera> camera) { m_current_camera = std::move(camera); }
    virtual std::shared_ptr<ICamera> getCurrentCamera() { return m_current_camera; }
};

#endif //RENDERER_H
