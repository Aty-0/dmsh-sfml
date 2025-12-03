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
        create(size, nullptr);
    }

    void PostEffectManager::create(const sf::Vector2u& size, std::shared_ptr<sf::Shader> shader)
    {
        const auto renderTexture = std::make_shared<sf::RenderTexture>(size);
        renderTexture->setSmooth(true);
        
        const auto effect = std::make_shared<PostEffect>();
        effect->renderTexture = renderTexture;
        effect->shader = shader;

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
    
            currentEffect->renderTexture->clear();
            currentEffect->renderTexture->draw(sf::Sprite { prevTexture->getTexture() }, currentEffect->shader.get());
            currentEffect->renderTexture->display();

            prevTexture = currentEffect->renderTexture;                    
        }       
    }

    void PostEffectManager::draw(sf::RenderTarget& window)
    {
        auto& finalEffect = m_postEffects.back();
        window.draw(sf::Sprite { finalEffect->renderTexture->getTexture() });                    
    }
}