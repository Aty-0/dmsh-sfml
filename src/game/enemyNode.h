#pragma once 
#include "../core/sceneManager.h"
#include "../core/component.h"
#include "../core/inputManager.h"
#include "../core/coroutine.h"

namespace dmsh::game
{    
    class NodeEditor; 
    class EnemyNode : public core::Component
    { 
        public:
            EnemyNode() : 
                m_isSelected(false), 
                m_nodeEditor(nullptr)
            {
                
            }
            
            virtual void onMouseUnselected(const sf::Vector2f& pos) override;
            virtual void onMouseSelected(const sf::Vector2f& pos) override;
        private:
            void onIsSelectedChanged();
            
            core::coroutines::Coroutine popupAnimation();
            core::coroutines::Coroutine popoutAnimation();

            std::shared_ptr<core::coroutines::Coroutine> m_animationCoroutine;
            
            friend class NodeEditor; 
            NodeEditor* m_nodeEditor;
            
            bool m_isSelected;
            bool m_useSpline;
            bool m_isFirstNode;
            bool m_isEndNode;
            
            // TODO: onReachAction, when enemy is reached node we need do something  
    };
}