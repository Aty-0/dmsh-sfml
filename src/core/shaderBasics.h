#pragma once
#include "common.h"

#include "window.h"
#include "time.h"

namespace dmsh::core
{
    inline static void updateShaderBaseUniforms(sf::Shader& shader)
    {
        static const auto time = Time::getInstance();
        static const auto window = Window::getInstance();

        shader.setUniform("u_time", time->getTime());
        shader.setUniform("u_resolution", static_cast<sf::Vector2f>(window->getSize()));
    }
}