//
// Created by curo on 30.01.2025.
//

#ifndef RENDERER_H
#define RENDERER_H
#include <chrono>
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
    float rotation = 0.0f;
};

using DrawLayer = std::unordered_map<uint32_t, DrawInfo>;

using DrawLayout = struct {
    std::vector<DrawLayer> layers;
    uint64_t build_time;
};

// using DrawLayout = std::vector<DrawLayer>;


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
        uint64_t current_time,
        float camera_height) {

        // auto current_time = std::chrono::steady_clock::now();
        // float logic_interval_seconds = std::chrono::duration<float>(current.build_time - previous.build_time).count();
        // float delta_since_logic_update = std::chrono::duration<float>(
        //     current_time - m_start_frame_processing_time).count();
        //
        // float interpolation_alpha = delta_since_logic_update / logic_interval_seconds;
        const uint64_t delta_time = current.build_time - previous.build_time;
        const uint64_t time_left = current_time - previous.build_time - delta_time;
        const float dt_ratio = static_cast<float>(time_left) / static_cast<float>(delta_time);

        for (const auto [
            previous_layer,
            current_layer] :
                std::views::zip(previous.layers, current.layers)) {
            drawLayer(previous_layer, current_layer, dt_ratio, camera_height);
        }
    }

    static std::unique_ptr<IRenderer> createRenderer(RendererType type);

    [[nodiscard]] virtual ITextureManager* getTextureManager() = 0;

    virtual void setViewPortSize(int width, int height) = 0;
    virtual void updateViewPortSize() = 0;
    virtual glm::ivec2 getViewPortSize() = 0;

    virtual void setCurrentCamera(std::shared_ptr<ICamera> camera) { m_current_camera = std::move(camera); }
    virtual std::shared_ptr<ICamera> getCurrentCamera() { return m_current_camera; }
};

#endif //RENDERER_H
