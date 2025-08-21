#pragma once
#include "../core/common.h"
#include "../core/component.h"
#include "../core/collider.h"
#include "../core/gameObject.h"
#include "../core/sceneManager.h"

namespace dmsh::game
{
    class Bullet : public core::Component
    {
        public:
            Bullet() : 
                m_speed(400.0f)
            {

            }

            virtual void onStart() override
            {
                auto owner = getOwner();

                auto& shape = owner->getDrawable()->create<sf::RectangleShape>();
                shape.setSize({20.0f, 20.0f});
                shape.setFillColor(sf::Color::White);

                owner->setTag("bullet");
                auto collider = owner->createComponent<core::RectangleCollider>();
                collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 20.0f, 20.0f } });
            }
 
            virtual void onUpdate(float delta) override
            {
                const auto owner = getOwner();

                if (owner != nullptr)
                {
                    auto transform = owner->getTransform();
                    auto pos = transform->getPosition();
                    transform->setPosition({pos.x, pos.y - (m_speed * delta)});
                }
            }   
        private: 
            float m_speed;
            // TODO: Bullet patterns
    };
}