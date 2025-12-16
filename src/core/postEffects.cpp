#include "postEffects.h"
#include "logger.h"
#include "shaderBasics.h"

namespace dmsh::core
{
    PostEffectManager::PostEffectManager() : m_isEnable(true)
    {

    }

    PostEffectManager::~PostEffectManager()
    {
        m_postEffects.clear();
        m_postEffects.shrink_to_fit();
    }

    void PostEffectManager::createCanvas(const sf::Vector2u& size)
    { 
        m_size = size;
        create(nullptr);
    }

    void PostEffectManager::resize(const sf::Vector2u& size)
    {
        m_size = size;
        
        for (std::int32_t i = 0; i < m_postEffects.size(); ++i) 
        {
            DMSH_DEBUG("Resize render texture -> %i", i);
            auto currentEffect = m_postEffects[i];
            auto texture = currentEffect->renderTexture;
            if (texture)
            {
                DMSH_ASSERT(texture->resize(m_size), "Failed to resize render texture!");
                currentEffect->sprite->setTexture(texture->getTexture(), true);
                currentEffect->sprite->setTextureRect({ { 0, 0 }, { static_cast<std::int32_t>(m_size.x), static_cast<std::int32_t>(m_size.y) }});
            }
        }
/*
        std::vector<std::shared_ptr<sf::Shader>> shaders;
        for (std::int32_t i = 1; i < m_postEffects.size(); ++i)
        {
            auto currentEffect = m_postEffects[i];
            shaders.push_back(currentEffect->shader);
        }
        
        m_postEffects.clear();
        m_postEffects.shrink_to_fit();
        
        createCanvas(size);
        for (const auto& shader : shaders) 
        {
            create(shader);
        }*/
    }

    void PostEffectManager::create(std::shared_ptr<sf::Shader> shader)
    {
        const auto renderTexture = std::make_shared<sf::RenderTexture>(m_size);
        renderTexture->setSmooth(true);
        
        const auto effect = std::make_shared<PostEffect>();
        effect->renderTexture = renderTexture;
        effect->shader = shader;

        effect->sprite = std::make_shared<sf::Sprite>(renderTexture->getTexture());
        effect->sprite->setTextureRect({ { 0, 0 }, { static_cast<std::int32_t>(m_size.x), static_cast<std::int32_t>(m_size.y) }});
        
        m_postEffects.push_back(effect);
    }

    std::shared_ptr<sf::RenderTexture> PostEffectManager::begin() 
    {
        if (m_postEffects.empty()) 
            return nullptr;      

        const auto effect = m_postEffects[0];
        DMSH_ASSERT(effect, "Invalid first effect");
        effect->renderTexture->clear();
        return effect->renderTexture;
    }

    void PostEffectManager::end()
    {
        if (m_postEffects.empty()) 
            return;

        // First effect pass ending
        const auto firstEffect = m_postEffects[0];
        DMSH_ASSERT(firstEffect, "Invalid first effect");

        firstEffect->renderTexture->display();

        std::shared_ptr<sf::RenderTexture> prevTexture = firstEffect->renderTexture;
        for (std::int32_t i = 1; i < m_postEffects.size(); ++i) 
        {
            auto currentEffect = m_postEffects[i];
            if (!currentEffect || !currentEffect->shader)
                continue;

            core::updateShaderBaseUniforms(*currentEffect->shader);
            currentEffect->shader->setUniform("u_texture", prevTexture->getTexture());
    
            auto& prevEffect = m_postEffects[i - 1];
            currentEffect->renderTexture->clear();
            currentEffect->renderTexture->draw(*prevEffect->sprite, currentEffect->shader.get());
            currentEffect->renderTexture->display();
            
            prevTexture = currentEffect->renderTexture;                    
        }       
    }

    void PostEffectManager::draw(sf::RenderTarget& window)
    {
        auto& finalEffect = m_postEffects.back();
        window.draw(*finalEffect->sprite);                    
    }
}