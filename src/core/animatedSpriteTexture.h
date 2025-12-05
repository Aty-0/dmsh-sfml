#pragma once 
#include "common.h"
#include "component.h"
#include "coroutine.h"
#include "logger.h"

namespace dmsh::core
{
    class AnimatedSpriteTexture : public Component
    {
        public:
            explicit AnimatedSpriteTexture(sf::Sprite& sprite, const std::initializer_list<sf::IntRect>& textureRectList, float speed) : 
                m_sprite(sprite),
                m_speed(speed) 
            {
                setRectList(textureRectList);
            } 
            
            void setRectList(const std::initializer_list<sf::IntRect>& textureRectList);
            void run();
            void stop();
            
        private:
            core::coroutines::Coroutine moveRect();

            std::shared_ptr<core::coroutines::Coroutine> m_animationCoroutine;
            sf::Sprite& m_sprite;
            std::vector<sf::IntRect> m_rectList;
            float m_speed;
    };
}