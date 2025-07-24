#pragma once 
#include "../core/sceneManager.h"
#include "../core/component.h"
#include "../core/inputManager.h"
#include "../core/debug.h"
#include "../core/window.h"
#include "../core/text.h"


#include "enemyNode.h"
#include <vector>

namespace dmsh::game
{    
    struct Pattern 
    {
        std::vector<std::shared_ptr<EnemyNode>> Nodes;
    };

    class NodeEditor : public core::Component
    {
        public:
            NodeEditor() : 
                m_currentPatternIndex(0), 
                m_onEditMode(false)                
            {            
   
            }  

            virtual void onStart() override;
            virtual void onInput(core::InputManager& input) override;            
            virtual void onRender(sf::RenderWindow& window) override;            
            virtual void onMouseClicked(const sf::Vector2f& pos) override;

            inline bool isOnEditMode() const { return m_onEditMode; }
            inline std::shared_ptr<EnemyNode> getSelected() const { return m_selected; }
            
            inline void setSelected(std::shared_ptr<EnemyNode> node) 
            {
                m_selected = node;
                updateSelectedText();
            }

        private:
            void createUI();
            void createNewPattern();            
            void updatePatternText();            
            void updateSelectedText();
            void switchToNextPattern();
            void switchToBackPattern();
            void moveNode();
            void setVisibilityNodes(std::vector<std::shared_ptr<EnemyNode>>& nodes, bool visible);

            std::int32_t m_currentPatternIndex;
            std::shared_ptr<Pattern> m_currentPattern;
            std::vector<std::shared_ptr<Pattern>> m_patterns;
            
            std::shared_ptr<EnemyNode> m_selected;
            bool m_onEditMode;

            std::shared_ptr<core::Text> m_patternTextComp;
            std::shared_ptr<core::Text> m_selectedObjectTextComp;
            
            const sf::Color DEFAULT_TEXT_COLOR = sf::Color(255, 255, 255, 150);
    };
}