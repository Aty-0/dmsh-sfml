#pragma once
#include "../core/common.h"
#include "../core/component.h"

namespace dmsh::game
{
    class Bullet : public core::Component
    {
        public:
            Bullet() : 
                m_speed(400.0f)
            {

            }

            virtual void onStart() override;
            virtual void onUpdate(float delta) override;
            
        private: 
            std::shared_ptr<sf::Shader> m_shader;
            float m_speed;
            // TODO: Bullet patterns
    };
}