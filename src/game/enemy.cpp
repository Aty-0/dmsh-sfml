#include "enemy.h"
#include "../core/gameObject.h"
#include "../core/inputManager.h"
#include "../core/sceneManager.h"
#include "../core/logger.h"

namespace dmsh::game
{
    void Enemy::onStart() 
    {
        auto owner = getOwner();
        static const auto resourceManager = core::ResourceManager::getInstance();
        const auto enemyTexture = resourceManager->get<core::ResourceTypes::Texture>("enemy_1");        
        DMSH_ASSERT(enemyTexture, "enemy texture is invalid");        
        auto& shape = owner->getDrawable()->create<sf::Sprite>(*enemyTexture->getHandle());
        shape.setTextureRect({{0, 0}, {30, 30}});
        
        owner->getTransform()->setPosition({500, 200});
        owner->setTag("enemy");
        auto collider = owner->createComponent<core::RectangleCollider>();
        collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 30.0f, 50.0f } });
        m_source = owner->createComponent<core::SoundSource>();
        m_source->setSound("damage");
    }

    void Enemy::onUpdate(float delta)
    {

    }

    void Enemy::onCollisionEnter(const core::Collider& collider)
    { 
        // Check tag from collider owner
        auto tag = collider.getOwner()->getTag();
        
        if (tag == "player" || tag == "bullet")
        {
            //DMSH_DEBUG("delete shit");
            auto sound = m_source->getSound();
            if (sound)
            {
                sound->setVolume(10.0f);         
                m_source->play(tag == "bullet");                       
            }
            
            // FIXME: 
            if (tag == "bullet")                    
                collider.getOwner()->setVisible(false);
            
            //static const auto sceneManager = core::SceneManager::getInstance();
            //sceneManager->deleteGameObject(getOwner());
        }
    }       
}