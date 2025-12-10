#pragma once 
#include "common.h"

namespace dmsh::core::debug
{
    enum class DebugRenderType
    {
        Line,
        Arrow,
        Square,
    };

    template<DebugRenderType Type>
    struct DebugTraits { };

    template<DebugRenderType Type>
    using Debug = DebugTraits<Type>;

    template<>
    struct DebugTraits<DebugRenderType::Arrow>
    {
        public:
            static void draw(sf::RenderTarget& window, const sf::Vector2f& first, const sf::Vector2f& end, const sf::Color& color, float headSize = 25.0f) 
            {
                const float width = headSize * 0.6f;
                const auto v = (end - first);
                const auto normal = v.length() == 0 ? v.normalized() : v;
                const auto perp1 = normal.perpendicular();
                const auto perp2 = -normal.perpendicular();

                const float headLength = 0.1f;
                const sf::Vertex triangle[] = 
                {
                    { end, color, { 0.0f , 0.0f } }, 
                    { end - headSize * (normal + perp1).normalized(), color, { 0.0f , 0.0f } }, 
                    { end - headSize * (normal + perp2).normalized(), color, { 0.0f , 0.0f } }, 
                };

                const sf::Vertex line[] =
                {
                    { first, color, { 0.0f , 0.0f } }, 
                    { end, color, { 0.0f , 0.0f } } 
                };

                window.draw(triangle, 3, sf::PrimitiveType::Triangles);
                window.draw(line, 4, sf::PrimitiveType::Lines);
            }
    };

    template<>
    struct DebugTraits<DebugRenderType::Line>
    {
        public:
            static void draw(sf::RenderTarget& window, const sf::Vector2f& first, const sf::Vector2f& end, const sf::Color& color) 
            {
                const sf::Vertex line[] =
                {
                    { first, color, { 0.0f , 0.0f } }, 
                    { end, color, { 0.0f , 0.0f } } 
                };

                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
    };

    template<>
    struct DebugTraits<DebugRenderType::Square>
    {
        public:
            static void draw(sf::RenderTarget& window, const sf::Vector2f& pos, const sf::Vector2f size, const sf::Color& color) 
            {
                const sf::Vertex square[] =
                {
                    { sf::Vector2(1.0f, 1.0f), color, { 0.0f , 0.0f } }, 
                    { sf::Vector2(1.0f, 0.0f), color, { 0.0f , 0.0f } }, 
                    { sf::Vector2(0.0f, 0.0f), color, { 0.0f , 0.0f } }, 
                    { sf::Vector2(0.0f, 1.0f), color, { 0.0f , 0.0f } }, 
                    { sf::Vector2(1.0f, 1.0f), color, { 0.0f , 0.0f } }, 
                };

                sf::RenderStates states; 
                sf::Transform transform;

                transform.translate(pos);
                transform.scale(size);
                states.transform = transform;

                window.draw(square, 5, sf::PrimitiveType::LineStrip, states);
            }
    };
}