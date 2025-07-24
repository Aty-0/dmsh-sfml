#pragma once
#include "../core/common.h"
#include "../core/component.h"
#include "../core/collider.h"
#include "../core/gameObject.h"
#include "../core/inputManager.h"
#include "../core/sceneManager.h"

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
            
            virtual void onStart() override
            {
                auto owner = getOwner();
    
                auto& shape = owner->getDrawable().create<sf::RectangleShape>();
                shape.setSize({30, 50});
                shape.setFillColor(sf::Color::White);
                
                owner->getTransform().setPosition({500, 200});
                owner->setTag("enemy");

                auto collider = owner->createComponent<core::RectangleCollider>();
                collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 30.0f, 50.0f } });
            }

            virtual void onUpdate(float delta) override
            {

            }

            virtual void onCollisionEnter(const core::Collider& collider)
            { 
                // Check tag from collider owner
                auto tag = collider.getOwner()->getTag();

                if (tag == "player" || tag == "bullet")
                {
                    DMSH_DEBUG("delete shit");
                    const static auto sceneManager = core::SceneManager::getInstance();
                    sceneManager->deleteGameObject(getOwner());
                }
            }

        private:
            float m_speed;
            float m_health;
    };
}
