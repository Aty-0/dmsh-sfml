#pragma once
#include "../core/common.h"
#include "../core/component.h"
#include "../core/collider.h"
#include "../core/gameObject.h"
#include "../core/sceneManager.h"
#include "../core/resourceManager.h"

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
                static const auto resourceManager = core::ResourceManager::getInstance();
                
                const auto bulletTexture = resourceManager->get<core::ResourceTypes::Texture>("bullet");
                DMSH_ASSERT(bulletTexture, "bullet texture is invalid");        
                auto& shape = owner->getDrawable()->create<sf::Sprite>(*bulletTexture->getHandle());
                shape.setColor({150, 50, 50, 255});

                owner->setTag("bullet");
                owner->getTransform()->setScale({0.5f, 0.5f});

                auto collider = owner->createComponent<core::RectangleCollider>();
                collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 64.0f, 64.0f } });
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