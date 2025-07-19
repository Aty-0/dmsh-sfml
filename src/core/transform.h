#pragma once 
#include "common.h"
#include "component.h"

#include <functional>

namespace dmsh::core
{
    struct Transform : public Component 
    {
        Transform() : 
            m_position(0.0f,0.0f), 
            m_rotation(), 
            m_scale(1.0f,1.0f)
        {

        }
        
        inline void invokeCallbacks() 
        {
            if (onPositionChanged) 
                onPositionChanged(m_position); 

            if (onRotationChanged) 
                onRotationChanged(m_rotation); 

            if (onScaleChanged) 
                onScaleChanged(m_scale); 
        }        

        inline void setPosition(const sf::Vector2f& pos) 
        {
            m_position = pos; 
            if (onPositionChanged) 
                onPositionChanged(m_position); 
        } 
        inline void setRotation(const sf::Angle& rotation) 
        { 
            m_rotation = rotation; 
            if (onRotationChanged) 
                onRotationChanged(m_rotation); 

        } 
        
        inline void setScale(const sf::Vector2f& scale) 
        { 
            m_scale = scale; 
            if (onScaleChanged) 
                onScaleChanged(m_scale); 
        }

        inline sf::Vector2f getPosition() const { return m_position; }
        inline sf::Angle getRotation() const { return m_rotation; }
        inline sf::Vector2f getScale() const { return m_scale; }
        private:
            sf::Vector2f m_position;
            sf::Angle m_rotation;
            sf::Vector2f m_scale;
        public:
            std::function<void(const sf::Vector2f&)> onPositionChanged;
            std::function<void(const sf::Angle&)> onRotationChanged;
            std::function<void(const sf::Vector2f&)> onScaleChanged;
    };
}