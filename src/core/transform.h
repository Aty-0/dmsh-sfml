#pragma once 
#include "common.h"
#include "component.h"

#include <functional>

namespace dmsh::core
{
    class Transform : public Component 
    {
        public:
            Transform() : 
                m_position(0.0f,0.0f), 
                m_rotation(), 
                m_scale(1.0f,1.0f)
            {

            }

            inline void invokeCallbacks() 
            {
                Signals.onPositionChanged(m_position); 
                Signals.onRotationChanged(m_rotation); 
                Signals.onScaleChanged(m_scale); 
            }        

            inline void setPosition(const sf::Vector2f& pos) 
            {
                m_position = pos; 
                Signals.onPositionChanged(m_position); 
            } 
            inline void setRotation(const sf::Angle& rotation) 
            { 
                m_rotation = rotation;             
                Signals.onRotationChanged(m_rotation); 
            } 

            inline void setScale(const sf::Vector2f& scale) 
            { 
                m_scale = scale;         
                Signals.onScaleChanged(m_scale); 
            }

            inline sf::Vector2f getPosition() const { return m_position; }
            inline sf::Angle getRotation() const { return m_rotation; }
            inline sf::Vector2f getScale() const { return m_scale; }
        private:
            sf::Vector2f m_position;
            sf::Angle m_rotation;
            sf::Vector2f m_scale;

        public:
            struct TransformSignals
            {
                sigslot::signal<const sf::Vector2f&> onPositionChanged;
                sigslot::signal<const sf::Angle&> onRotationChanged;
                sigslot::signal<const sf::Vector2f&> onScaleChanged;
            };
            TransformSignals Signals;
    };
}