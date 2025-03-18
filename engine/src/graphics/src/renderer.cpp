//
// Created by curo on 30.01.2025.
//

#include "renderer.hpp"

#include "opengl/opengl_renderer.hpp"

std::unique_ptr<IRenderer> IRenderer::createRenderer(const RendererType type) {
    switch (type) {
    case RendererType::OPEN_GL:
        return std::make_unique<OpenGLRenderer>();
    default:
        return nullptr;
    }
}
