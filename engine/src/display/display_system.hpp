//
// Created by curo on 08.02.2025.
//

#ifndef DISPLAY_SYSTEM_H
#define DISPLAY_SYSTEM_H
#include "renderer.hpp"
#include <atomic>
#include <mutex>

enum class Layers {
    Terrain = 0,
    Land = 1,
    Object = 2,
    Air = 3,
    Total = 4
};

class DisplaySystem {
    std::unique_ptr<DrawLayout> m_last_layout;
    std::unique_ptr<DrawLayout> m_prev_layout;
    std::vector<DrawLayer> m_building_layout;
    // std::unique_ptr<DrawLayout> m_temp_layout;

    std::atomic_bool m_is_need_to_shift;
    std::mutex& m_mutex;

    IRenderer* m_renderer;

    void shiftLayout(uint64_t current_time) {
        // if (m_temp_layout) {
        //     m_last_layout = std::move(m_prev_layout);
        //     m_prev_layout = std::move(m_temp_layout);
        //     m_temp_layout = nullptr;
        // }
        std::lock_guard lock(m_mutex);
        m_prev_layout = std::move(m_last_layout);
        m_last_layout = std::make_unique<DrawLayout>(std::move(m_building_layout), current_time);
        m_building_layout = std::vector<DrawLayer>(k_max_layers);
    }

public:
    static constexpr uint32_t k_max_layers = static_cast<uint32_t>(Layers::Total);

    explicit DisplaySystem(std::mutex &mutex, uint64_t current_time)
        : m_is_need_to_shift(false), m_mutex(mutex), m_renderer(nullptr), m_building_layout(k_max_layers) {
        m_last_layout = std::make_unique<DrawLayout>(std::vector<DrawLayer>(k_max_layers), current_time);
        m_prev_layout = std::make_unique<DrawLayout>(std::vector<DrawLayer>(k_max_layers), current_time);
    }

    void init(IRenderer *renderer) {
        m_renderer = renderer;
    }

    ~DisplaySystem() = default;

    [[nodiscard]] std::vector<DrawLayer> &getBuildingLayout() { return m_building_layout; }

    void nextState(uint64_t current_time) {
        // std::lock_guard lock(m_mutex);
        // m_is_need_to_shift.test_and_set();
        // m_temp_layout = std::move(m_building_layout);
        // m_building_layout = std::make_unique<DrawLayout>(k_max_layers);
        shiftLayout(current_time);
    }

    void draw(const uint64_t current_time, const float camera_height) {
        // if (m_is_need_to_shift.test()) {
        //     std::lock_guard lock(m_mutex);
        //     m_is_need_to_shift.clear();
        //     shiftLayout();
        // }
        std::lock_guard lock(m_mutex);
        m_renderer->drawLayout(*m_prev_layout, *m_last_layout, current_time, camera_height);
    }

};

#endif //DISPLAY_SYSTEM_H
