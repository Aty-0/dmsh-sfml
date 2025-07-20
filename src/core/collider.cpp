#include "collider.h"
#include "gameObject.h"
#include "debug.h"

namespace dmsh::core
{
    static const auto updateRectByTransform = [](const RectangleCollider& collider)
    {
        const auto go = collider.getOwner();
        const auto transform = go->getTransform();
        const auto pos = transform.getPosition();
        const auto scale = transform.getScale();
        
        auto rectUpd = collider.getRect();
        rectUpd.position.x += pos.x;
        rectUpd.position.y += pos.y;
        rectUpd.size.x *= scale.x;
        rectUpd.size.y *= scale.y;
        return rectUpd;
    };

    bool RectangleCollider::contains(const sf::Vector2f& pos) const 
    {                
        auto rect1 = updateRectByTransform(*this);
        return rect1.contains(pos);
    }

    bool RectangleCollider::intersect(const Collider& collider) const
    {
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

    void RectangleCollider::onRender(sf::RenderWindow& window) 
    {
        if (m_showRect)
        {
            auto rect = updateRectByTransform(*this);
            debug::Square::draw(window, rect.position, rect.size, sf::Color::Magenta);
        }
    }
        
    
}