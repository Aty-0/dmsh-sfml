#pragma once
#include "common.h"
#include "singleton.h"

namespace dmsh::core
{    
    class PostEffectManager : public Singleton<PostEffectManager> 
    {
        public:
            struct PostEffect
            {
                std::shared_ptr<sf::RenderTexture> renderTexture;
                std::shared_ptr<sf::Shader> shader;
                std::shared_ptr<sf::Sprite> sprite;
            };

            PostEffectManager();
            ~PostEffectManager();

            // Create a empty effect with render texture which needed to get rendered scene image  
            void createCanvas(const sf::Vector2u& size);
            void resize(const sf::Vector2u& size);
            void create(std::shared_ptr<sf::Shader> shader);

            // TODO: void updateRenderTexturesSize(const sf::Vector2u& newSize);

            std::shared_ptr<sf::RenderTexture> begin();
            void end();
            void draw(sf::RenderTarget& window);

            inline std::shared_ptr<PostEffect> getPostEffect(std::int32_t index) const 
            { 
                return m_postEffects[index]; 
            }

            inline void setEnable(bool isEnable) { m_isEnable = isEnable; }
            inline bool isEnable() const { return m_isEnable || m_postEffects.size() <= 1; }
            
            inline std::vector<std::shared_ptr<PostEffect>> getPostEffects() const { return m_postEffects; }

        private:
            sf::Vector2u m_size;
            bool m_isEnable;
            std::vector<std::shared_ptr<PostEffect>> m_postEffects;
    };

}