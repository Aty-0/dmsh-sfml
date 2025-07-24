#include "enemyNode.h"
#include "nodeEditor.h"

#include "../core/time.h"

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
        const auto drawable = getOwner()->getDrawable().get<sf::RectangleShape>();
        const static auto coroutineScheduler = core::coroutines::CoroutineScheduler::getInstance();
        
        coroutineScheduler->stop(m_animationCoroutine);
        
        if (m_isSelected)
        {
            drawable->setFillColor(sf::Color::Red);        
            m_nodeEditor->setSelected(shared); 
            
            m_animationCoroutine = coroutineScheduler->run(std::bind(&EnemyNode::popupAnimation, this));
        }
        else
        {
            drawable->setFillColor(sf::Color::White);        
            m_nodeEditor->setSelected(nullptr);             
            m_animationCoroutine = coroutineScheduler->run(std::bind(&EnemyNode::popoutAnimation, this));
        }
    }
    
    static constexpr float MaxNodeSize = 1.3f;
    static constexpr float SpeedPop = 10.0f;

    core::coroutines::Coroutine EnemyNode::popoutAnimation()
    {
        const auto owner = getOwner();
        auto& transform = owner->getTransform();
        auto scale = transform.getScale();
        static const auto time = core::Time::getInstance();

        float s = MaxNodeSize;
        while (s > 1.0f)
        {
            s -= SpeedPop * time->getDelta();
            transform.setScale({s, s});

            co_await core::coroutines::Continue();
        }
    }

    core::coroutines::Coroutine EnemyNode::popupAnimation()
    {
        const auto owner = getOwner();
        auto& transform = owner->getTransform();
        static const auto time = core::Time::getInstance();
        float s = 1.0f;
        while (s < MaxNodeSize)
        {
            s += SpeedPop * time->getDelta();
            transform.setScale({s, s});

            co_await core::coroutines::Continue();
        }
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