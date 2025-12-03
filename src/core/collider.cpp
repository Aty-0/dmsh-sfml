#include "collider.h"
#include "gameObject.h"
#include "debug.h"
#include "spatialGrid.h"

namespace dmsh::core
{
    static const auto grid = CollisionSpatialGrid::getInstance();

    static void updateCollisionGrid(std::shared_ptr<Collider> col) 
    {
        DMSH_ASSERT(col, "update collision grid failed");
        grid->addColliderToCells(col);
    }
    
    Collider::~Collider()
    {
        m_cells.clear();
    }

    void Collider::onDestroy()
    {
        grid->removeColliderFromCells(getSelf<Collider>());
    } 

    void Collider::onStart() 
    {
        const auto owner = getOwner();
        const auto transform = owner->getTransform();
        
        const auto self = getSelf<Collider>();
        auto update = [self](const sf::Vector2f&) 
        { 
            updateCollisionGrid(self); 
        };

        transform->Signals.onPositionChanged.connect(update);
    }

    static auto updateRectByTransform(const RectangleCollider& collider)
    {
        const auto go = collider.getOwner();
        const auto transform = go->getTransform();
        const auto pos = transform->getPosition();
        const auto scale = transform->getScale();
        
        auto rectUpd = collider.getRect();
        rectUpd.position.x += pos.x;
        rectUpd.position.y += pos.y;
        rectUpd.size.x *= scale.x;
        rectUpd.size.y *= scale.y;
        return rectUpd;
    }
    
    bool RectangleCollider::m_alwaysShowRect;

    bool RectangleCollider::contains(const sf::Vector2f& pos) const 
    {       
        if (!getOwner()->isVisible())
            return false;

        auto rect1 = updateRectByTransform(*this);
        return rect1.contains(pos);
    }

    bool RectangleCollider::intersect(const sf::FloatRect& rect) const
    {     
        auto rect1 = updateRectByTransform(*this);
        return rect1.findIntersection(rect).has_value();
    }

    bool RectangleCollider::intersect(const Collider& collider) const
    {
        // Don't do any intersect if owner for both sides is not visible 
        if (!getOwner()->isVisible() 
            || !collider.getOwner()->isVisible())
            return false;

        auto rectCollider = dynamic_cast<const RectangleCollider*>(&collider);        
        if (rectCollider == nullptr)
        {
            DMSH_ERROR("Rectangle collider is not support for other types colliders yet. (bad cast error)");
            return false;
        }
        
        auto rect1 = updateRectByTransform(*this);
        auto rect2 = updateRectByTransform(*rectCollider);
        
        return rect1.findIntersection(rect2).has_value();
    }

    void RectangleCollider::onRender(sf::RenderTarget& window) 
    {
        if (m_showRect || m_alwaysShowRect)
        {
            auto rect = updateRectByTransform(*this);
            debug::Square::draw(window, rect.position, rect.size, sf::Color::Magenta);
        }
    }
        
    
}