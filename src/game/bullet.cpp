#include "bullet.h"
#include "../core/collider.h"
#include "../core/gameObject.h"
#include "../core/sceneManager.h"
#include "../core/resourceManager.h"
#include "../core/time.h"
#include "../core/shaderBasics.h"

namespace dmsh::game
{
    void Bullet::onStart()
    {
        static const auto resourceManager = core::ResourceManager::getInstance();
        
        auto owner = getOwner();
        const auto bulletTexture = resourceManager->get<core::ResourceTypes::Texture>("bullet");
        DMSH_ASSERT(bulletTexture, "bullet texture is invalid");        

        auto drawable = owner->getDrawable();
        auto& shape = drawable->create<sf::Sprite>(*bulletTexture->getHandle());
        shape.setColor({150, 50, 50, 255});
        
        const auto shader = resourceManager->get<core::ResourceTypes::Shader>("pt");
        DMSH_ASSERT(shader, "bullet shader is invalid");

        m_shader = shader->getHandle();
        m_shader->setUniform("texture", *bulletTexture->getHandle());                
        drawable->setShader(*m_shader);

        auto& states = drawable->getRenderStates();
        states.blendMode = sf::BlendAdd;

        owner->setTag("bullet");
        owner->getTransform()->setScale({0.5f, 0.5f});
        auto collider = owner->createComponent<core::RectangleCollider>();
        collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 64.0f, 64.0f } });
    }

    void Bullet::onUpdate(float delta)
    {
        core::updateShaderBaseUniforms(*m_shader);
        static const auto time = core::Time::getInstance();

        const auto owner = getOwner();
        const auto transform = owner->getTransform();
        const auto pos = transform->getPosition();
        transform->setPosition({pos.x, pos.y - (m_speed * delta)});        
    }  
}