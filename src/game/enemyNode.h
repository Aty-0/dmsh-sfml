#pragma once 
#include "../core/sceneManager.h"
#include "../core/component.h"
#include "../core/inputManager.h"

namespace dmsh::game
{    
    class NodeEditor; 
    class EnemyNode : public core::Component, 
        public std::enable_shared_from_this<EnemyNode>
    { 
        public:
            EnemyNode() : 
                m_isSelected(false), 
                m_nodeEditor(nullptr)
            {
                
            }

            virtual void onMouseSelected(const sf::Vector2f& pos) override;
        private:
            friend class NodeEditor; 
            NodeEditor* m_nodeEditor;
            bool m_isSelected;
    };
}