#include "enemyNode.h"
#include "nodeEditor.h"

#include "../core/time.h"

namespace dmsh::game
{
    void EnemyNode::onStart() 
    {
        //auto owner = getOwner();
        //owner->setViewSpace(game::getViewSpaceEditor());   
    }
    
    void EnemyNode::onMouseUnselected(const sf::Vector2f& pos)
    {
        if (m_nodeEditor->isWindowFocused() || m_nodeEditor->getEditorMode() != NodeEditor::EditorMode::Edit)
            return;

        const auto selected = m_nodeEditor->getSelected();
        const auto self = getSelf<EnemyNode>();  

        DMSH_DEBUG("Mouse is clicked on nothing");
        
        if (selected == self)
        {
            DMSH_DEBUG("Selected it's me, unselect");
            m_isSelected = false;
            onIsSelectedChanged();
        }
    }

    void EnemyNode::onIsSelectedChanged()
    {
        const auto self = getSelf<EnemyNode>();    
        const auto& drawable = getOwner()->getDrawable()->get<sf::Sprite>();
        static const auto coroutineScheduler = core::coroutines::CoroutineScheduler::getInstance();
        
        coroutineScheduler->stop(m_animationCoroutine);
        
        if (m_isSelected)
        {
            DMSH_DEBUG("It's selected");
            drawable->setColor(sf::Color::Red);        
            m_nodeEditor->setSelected(self); 
            
            m_animationCoroutine = coroutineScheduler->run(std::move(std::bind(&EnemyNode::popupAnimation, this)));
        }
        else
        {
            DMSH_DEBUG("It's unselected");
            drawable->setColor(sf::Color::White);        
            m_nodeEditor->setSelected(nullptr);             
            m_animationCoroutine = coroutineScheduler->run(std::move(std::bind(&EnemyNode::popoutAnimation, this)));
        }
    }
    
    static constexpr float MaxNodeSize = 1.3f;
    static constexpr float SpeedPop = 10.0f;

    core::coroutines::Coroutine EnemyNode::popoutAnimation()
    {
        const auto owner = getOwner();
        auto transform = owner->getTransform();
        auto scale = transform->getScale();
        static const auto time = core::Time::getInstance();

        float s = MaxNodeSize;
        while (s > 1.0f)
        {
            s -= SpeedPop * time->getDelta();
            transform->setScale({s, s});

            co_await core::coroutines::Continue();
        }
    }

    core::coroutines::Coroutine EnemyNode::popupAnimation()
    {
        const auto owner = getOwner();
        auto transform = owner->getTransform();
        static const auto time = core::Time::getInstance();
        float s = 1.0f;
        while (s < MaxNodeSize)
        {
            s += SpeedPop * time->getDelta();
            transform->setScale({s, s});

            co_await core::coroutines::Continue();
        }
    }

            
    void EnemyNode::onMouseSelected(const sf::Vector2f& pos)
    {
        if (m_nodeEditor->isWindowFocused() || m_nodeEditor->getEditorMode() != NodeEditor::EditorMode::Edit)
            return;
        
        const auto selected = m_nodeEditor->getSelected();
        const auto self = getSelf<EnemyNode>();
        
        // Unselect prev object
        if (selected != nullptr && selected != self)
        {
            DMSH_DEBUG("Unselect previous object...");
            selected->m_isSelected = false;
            selected->onIsSelectedChanged();
        }

        DMSH_DEBUG("Mouse clicked on node");
        m_isSelected = true;        
        onIsSelectedChanged();
    }
}