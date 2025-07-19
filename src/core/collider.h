#pragma once
#include "common.h"
#include "component.h"

// TODO: implement logic on collision, also make a static variant (like a trigger). 
// At this moment we are have basic detection and invoke callback 

namespace dmsh::core
{
    struct Collider : public Component
    {
        virtual bool contains(const sf::Vector2f& pos) const { return false; }
        virtual bool intersect(const Collider& collider) const { return false; }       
    };
    
    struct RectangleCollider : public Collider
    {
        virtual bool contains(const sf::Vector2f& pos) const override;
        virtual bool intersect(const Collider& collider) const override;        
        
        inline sf::FloatRect getRect() const { return m_rect; } 
        inline void setRect(const sf::FloatRect& rect) { m_rect = rect; }
        private:
            sf::FloatRect m_rect;
    };
}