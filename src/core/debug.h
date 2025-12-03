#pragma once 
#include "common.h"

namespace dmsh::core::debug
{
    class Line
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

    class Square
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