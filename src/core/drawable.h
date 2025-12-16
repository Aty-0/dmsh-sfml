#pragma once
#include "common.h"
#include "component.h"
#include <memory>

namespace dmsh::core
{
    struct Drawable : public Component
    {  
        virtual void onRender(sf::RenderTarget& window) override
        {
            if (m_drawable == nullptr)
                return;
            
            window.draw(*m_drawable, m_states);
        }
        
        template<typename T, typename... Args>
        inline T& create(Args&&... args)
        {
            // TODO: Remove prev drawable, transform
            static_assert(!std::is_base_of<T, sf::Drawable>::value && !std::is_base_of<T, sf::Transformable>::value, "Not a drawable based class");
            m_drawable = std::make_shared<T>(std::forward<Args>(args)...);
            m_transformable = std::dynamic_pointer_cast<sf::Transformable>(m_drawable);
            m_states = sf::RenderStates::Default;

            DMSH_ASSERT(m_transformable, "bad transformable cast");
            return *std::dynamic_pointer_cast<T>(m_drawable);
        } 

        inline void updatePositionForTransformable(const sf::Vector2f& pos) 
        {
            if (m_transformable == nullptr)
                return;

            m_transformable->setPosition(pos);
        } 

        inline void updateRotationForTransformable(const sf::Angle& rotation) 
        {             
            if (m_transformable == nullptr)
                return;

            m_transformable->setRotation(rotation);
        } 
        
        inline void updateScaleForTransformable(const sf::Vector2f& scale) 
        {             
            if (m_transformable == nullptr)
                return;
            
            m_transformable->setScale(scale);
        }
        
        template<typename T>
        inline std::shared_ptr<T> get() const 
        {
            static_assert(!std::is_base_of<T, sf::Drawable>::value, "Not a drawable based class");
            if (m_drawable == nullptr)
                return nullptr;

            return std::dynamic_pointer_cast<T>(m_drawable); 
        } 

        inline void setRenderStates(const sf::RenderStates& states)
        {
            m_states = states;
        }
        inline sf::RenderStates& getRenderStates() { return m_states; } 
        inline void setShader(const sf::Shader& shader) 
        {
            m_states.shader = &shader;
        }

        private:
        	sf::RenderStates m_states;
            std::shared_ptr<sf::Drawable> m_drawable;
            std::shared_ptr<sf::Transformable> m_transformable;
    };
}