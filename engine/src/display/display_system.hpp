//
// Created by curo on 08.02.2025.
//

#ifndef DISPLAY_SYSTEM_H
#define DISPLAY_SYSTEM_H
#include "renderer.hpp"

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
    std::unique_ptr<DrawLayout> m_building_layout;
    // std::unique_ptr<DrawLayout> m_temp_layout;

    std::atomic_flag m_is_need_to_shift;
    std::mutex& m_mutex;

    IRenderer* m_renderer;

    void shiftLayout() {
        // if (m_temp_layout) {
        //     m_last_layout = std::move(m_prev_layout);
        //     m_prev_layout = std::move(m_temp_layout);
        //     m_temp_layout = nullptr;
        // }
        std::lock_guard lock(m_mutex);
        m_prev_layout = std::move(m_last_layout);
        m_last_layout = std::move(m_building_layout);
        m_building_layout = std::make_unique<DrawLayout>(k_max_layers);
    }

public:
    static constexpr uint32_t k_max_layers = static_cast<uint32_t>(Layers::Total);

    explicit DisplaySystem(std::mutex &mutex)
        : m_is_need_to_shift(false), m_mutex(mutex), m_renderer(nullptr) {
        m_last_layout = std::make_unique<DrawLayout>(k_max_layers);
        m_prev_layout = std::make_unique<DrawLayout>(k_max_layers);
        m_building_layout = std::make_unique<DrawLayout>(k_max_layers);
    }

    void init(IRenderer *renderer) {
        m_renderer = renderer;
    }

    ~DisplaySystem() = default;

    [[nodiscard]] DrawLayout &getBuildingLayout() const { return *m_building_layout; }

    void nextState() {
        // std::lock_guard lock(m_mutex);
        // m_is_need_to_shift.test_and_set();
        // m_temp_layout = std::move(m_building_layout);
        // m_building_layout = std::make_unique<DrawLayout>(k_max_layers);
        shiftLayout();
    }

    void draw(const float dt_ratio, const float camera_height) {
        // if (m_is_need_to_shift.test()) {
        //     std::lock_guard lock(m_mutex);
        //     m_is_need_to_shift.clear();
        //     shiftLayout();
        // }
        std::lock_guard lock(m_mutex);
        m_renderer->drawLayout(*m_prev_layout, *m_last_layout, dt_ratio, camera_height);
    }

};

#endif //DISPLAY_SYSTEM_H
