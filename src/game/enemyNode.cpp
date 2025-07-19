#include "enemyNode.h"
#include "nodeEditor.h"

namespace dmsh::game
{
    void EnemyNode::onStart()
    {
        const auto inputManager = core::InputManager::getInstance();
        inputManager->addListener(core::InputListenerType::KeyHold, core::MouseButtons::Left, [this]() {        
            if (m_isSelected)
            {

            }
        });

    }

    void EnemyNode::onMouseSelected(const sf::Vector2f& pos)
    {
        if (!m_nodeEditor->isOnEditMode())
            return;
            
        auto selected = m_nodeEditor->getSelected();
        auto shared = shared_from_this();
        // Unselect prev object
        if (selected != nullptr)
        {
            const auto selectedDrawable = selected->getOwner()->getDrawable().getDrawable<sf::RectangleShape>();
            selectedDrawable->setFillColor(sf::Color::White);        
            selected->m_isSelected = false;
            return;
        }
        
        m_isSelected = !m_isSelected;        
        const auto drawable = getOwner()->getDrawable().getDrawable<sf::RectangleShape>();
        drawable->setFillColor(m_isSelected ? sf::Color::Red : sf::Color::White);        
        m_nodeEditor->setSelected(m_isSelected ? shared : nullptr);        
    }
}