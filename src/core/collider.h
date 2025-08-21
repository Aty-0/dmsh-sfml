#pragma once
#include "common.h"
#include "component.h"

#include <unordered_set>

namespace dmsh::core
{
    // The basic cell struct which store colliders in bounds
    struct CollisionCell
    {
        sf::FloatRect Bounds;
        sf::Vector2i Indices;
        std::vector<std::shared_ptr<Collider>> Colliders;
    };

    struct Collider : public Component
    {
        virtual void onStart() override;
        virtual void onDestroy() override;
        virtual bool contains(const sf::Vector2f& pos) const { return false; }
        virtual bool intersect(const Collider& collider) const { return false; }       
        virtual bool intersect(const sf::FloatRect& rect) const { return false; }       

        private:
            friend class CollisionSpatialGrid;
        
            struct ColliderTracker 
            {
                inline void addCollidedObject(std::shared_ptr<Collider> collider)
                {
                    m_collided.insert(collider);
                }
                
                inline bool isObjectCollidedPrevFrame(std::shared_ptr<Collider> collider) const
                {
                    return m_collidedInPrevFrame.count(collider) != 0;
                }

                inline bool isObjectCollided(std::shared_ptr<Collider> collider) const
                {
                    return m_collided.count(collider) != 0;
                }
                
                inline void updateFrame()
                {
                    m_collidedInPrevFrame = std::move(m_collided);
                    m_collided.clear();
                }

                inline std::unordered_set<std::shared_ptr<Collider>> getCollided() const { return m_collided; }
                inline std::unordered_set<std::shared_ptr<Collider>> getCollidedInPrevFrame() const { return m_collidedInPrevFrame; }

                private:
                    std::unordered_set<std::shared_ptr<Collider>> m_collided;
                    std::unordered_set<std::shared_ptr<Collider>> m_collidedInPrevFrame;
            };        

            // A storage where we are save a active cells for current collider
            std::unordered_set<std::shared_ptr<CollisionCell>> m_cells;

            // Tracker for colliders to detect which objects has been collided with owner
            ColliderTracker m_collisionTracker; 
    };

    // TODO: Implement triggers     
    struct RectangleCollider : public Collider
    {
        virtual bool contains(const sf::Vector2f& pos) const override;
        virtual bool intersect(const Collider& collider) const override;        
        virtual bool intersect(const sf::FloatRect& rect) const override;       
        virtual void onRender(sf::RenderWindow& window) override;

        inline sf::FloatRect getRect() const { return m_rect; } 
        inline void setRect(const sf::FloatRect& rect) { m_rect = rect; }

        inline void setShowRect(bool show) { m_showRect = show; }
        inline bool getShowRect() const { return m_showRect; }
        
        inline static void setAlwaysShowRect(bool show) { m_alwaysShowRect = show; }
        inline static bool getAlwaysShowRect() { return m_alwaysShowRect; }

        private:
            static bool m_alwaysShowRect;
            bool m_showRect;
            sf::FloatRect m_rect;
    };
}