#include "enemyNode.h"
#include "nodeEditor.h"

namespace dmsh::game
{
    void EnemyNode::onMouseUnselected(const sf::Vector2f& pos)
    {
        if (!m_nodeEditor->isOnEditMode())
            return;

        const auto selected = m_nodeEditor->getSelected();
        const auto shared = shared_from_this();
        
        if (selected == shared)
        {
            DMSH_DEBUG("unselect");
            m_isSelected = false;
            onIsSelectedChanged();
        }
    }

    void EnemyNode::onIsSelectedChanged()
    {
        auto shared = shared_from_this();
        const auto drawable = getOwner()->getDrawable().getDrawable<sf::RectangleShape>();
        drawable->setFillColor(m_isSelected ? sf::Color::Red : sf::Color::White);        
        m_nodeEditor->setSelected(m_isSelected ? shared : nullptr); 
    }

    void EnemyNode::onMouseSelected(const sf::Vector2f& pos)
    {
        if (!m_nodeEditor->isOnEditMode())
            return;
            
        const auto selected = m_nodeEditor->getSelected();
        const auto shared = shared_from_this();
        
        // Unselect prev object
        if (selected != nullptr && selected != shared)
        {
            DMSH_DEBUG("remove prev");
            selected->m_isSelected = false;
            selected->onIsSelectedChanged();
        }
        
        DMSH_DEBUG("select");
        m_isSelected = true;        
        onIsSelectedChanged();
    }
}