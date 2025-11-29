#pragma once
#include "../core/common.h"
#include "../core/component.h"
#include "../core/collider.h"
#include "../core/soundSource.h"

namespace dmsh::game
{
    struct Enemy : public core::Component
    {
        public:
            Enemy() : 
                m_speed(500.5f), 
                m_health(1.0f)
            {                
            }
            
            virtual void onStart() override;
            virtual void onUpdate(float delta) override;
            virtual void onCollisionEnter(const core::Collider& collider) override;

        private:
            float m_speed;
            float m_health;
            std::shared_ptr<core::SoundSource> m_source;
    };
}
