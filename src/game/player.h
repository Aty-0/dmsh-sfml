#pragma once
#include "../core/common.h"
#include "../core/gameObject.h"
#include "../core/inputManager.h"
#include "../core/coroutine.h"
#include "../core/animatedSpriteTexture.h"

#include "bullet.h"
#include "bulletPool.h"

namespace dmsh::game
{
    class Player : public core::Component
    {
        public:
            enum class AnimationState 
            {
                None,
                Idle, 
                MoveRight,
                MoveLeft,
            };

            Player() : 
                m_speed(500.5f), 
                m_move({0,0}),
                m_shootDuration(0.05f),
                m_canShoot(true)
            {
            }

            virtual void onStart() override;
           
            virtual void onInput(core::InputManager& input) override;  
            virtual void onDestroy() override;
            virtual void onUpdate(float delta) override;

            void setAnimation(const AnimationState& state);
        private:
            void stopShooting();
            void shoot();
            core::coroutines::Coroutine shootCoroutine();
            
            std::shared_ptr<core::AnimatedSpriteTexture> m_animatedTexture;
            std::shared_ptr<core::coroutines::Coroutine> m_shootCoroutine;
            bool m_canShoot;
            float m_shootDuration;
            float m_speed;
            sf::Vector2i m_move;
            
            std::shared_ptr<BulletPool> m_bulletPool;
    };
}
