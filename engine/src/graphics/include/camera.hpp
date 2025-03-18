//
// Created by curo on 07.02.2025.
//

#ifndef CAMERA_H
#define CAMERA_H
#include "opengl/opengl_graphics_constants.hpp"

class ICamera {
    float m_height = constants::k_start_camera_height;

public:
    ICamera() = default;
    explicit ICamera(float height) : m_height(height) {}
    virtual ~ICamera() = default;

    ICamera(ICamera&& other) noexcept
        : m_height(other.m_height)
    {
    }

    void setHeight(float height) {
        m_height = height;
        1;
    }
    [[nodiscard]] float getHeight() const { return m_height; }
};

#endif //CAMERA_H
