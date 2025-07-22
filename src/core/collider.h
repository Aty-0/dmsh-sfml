#pragma once
#include "common.h"
#include "component.h"

#include <unordered_set>
// TODO: implement logic on collision, also make a static variant (like a trigger). 
// At this moment we are have basic detection and invoke callback 

namespace dmsh::core
{
    struct Collider : public Component
    {
        virtual bool contains(const sf::Vector2f& pos) const { return false; }
        virtual bool intersect(const Collider& collider) const { return false; }       
    
        private:
            friend class SceneManager;

            struct ColliderTracker 
            {
                inline bool isObjectCollidedPrevFrame(Collider* collider) const
                {
                    return CollidedInPrevFrame.count(collider) != 0;
                }

                inline bool isObjectCollided(Collider* collider) const
                {
                    return Collided.count(collider) != 0;
                }
                
                std::unordered_set<Collider*> Collided;
                std::unordered_set<Collider*> CollidedInPrevFrame;
            };
            
            ColliderTracker m_collisionTracker; 
    };
    
    struct RectangleCollider : public Collider
    {
        virtual bool contains(const sf::Vector2f& pos) const override;
        virtual bool intersect(const Collider& collider) const override;        
        virtual void onRender(sf::RenderWindow& window) override;

        inline sf::FloatRect getRect() const { return m_rect; } 
        inline void setRect(const sf::FloatRect& rect) { m_rect = rect; }
        private:
            bool m_showRect;
            sf::FloatRect m_rect;
    };
}