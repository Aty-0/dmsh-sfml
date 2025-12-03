#pragma once 
#include "../core/sceneManager.h"
#include "../core/component.h"
#include "../core/text.h"
#include "../core/view.h"


#include "enemyNode.h"
#include <vector>

namespace dmsh::game
{    
    DECLARE_VIEW_SPACE(Editor, 3);
        
    struct Pattern 
    {
        std::vector<std::shared_ptr<EnemyNode>> Nodes;
    };

    struct JsonNode
    {
        bool isSelected;
        bool useSpline;
        bool isFirstNode;
        bool isEndNode;
        float x;
        float y;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(JsonNode, isSelected, useSpline, isFirstNode, isEndNode, x, y);
    };
    
    //NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonNode, isSelected, useSpline, isFirstNode, isEndNode, x, y);
    inline static void to_json(nlohmann::json& j, const JsonNode& node)
    {
        j = nlohmann::json
        {
            { "isSelected", node.isSelected },
            { "useSpline", node.useSpline },
            { "isFirstNode", node.isFirstNode },
            { "isEndNode", node.isEndNode }, 
            { "x", node.x }, 
            { "y", node.y }, 
        };
    }

    struct JsonPattern
    {
        std::vector<JsonNode> nodes;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(JsonPattern, nodes);
    };
    
    inline static void to_json(nlohmann::json& j, const JsonPattern& pattern)
    {
        j = nlohmann::json
        {
            { "nodes", pattern.nodes },
        };
    }

    //NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonPattern, nodes);

    class NodeEditor : public core::Component
    {
        public:
            NodeEditor() : 
                m_currentPatternIndex(0), 
                m_onEditMode(true)                
            {            
   
            }  
            
            virtual void onGui() override;
            virtual void onStart() override;
            virtual void onInput(core::InputManager& input) override;            
            virtual void onRender(sf::RenderTarget& window) override;            
            virtual void onMouseClicked(const sf::Vector2f& pos) override;

            inline bool isOnEditMode() const { return m_onEditMode; }
            inline std::shared_ptr<EnemyNode> getSelected() const { return m_selected; }
            
            inline void setSelected(std::shared_ptr<EnemyNode> node) 
            {
                m_selected = node;
            }

        private:
            void save(std::string_view name);
            void createNewPattern();            
            void switchPattern();
            void switchToNextPattern();
            void switchToBackPattern();
            void moveNode();
            void setVisibilityNodes(std::vector<std::shared_ptr<EnemyNode>>& nodes, bool visible);

            std::int32_t m_currentPatternIndex;
            std::shared_ptr<Pattern> m_currentPattern;
            std::vector<std::shared_ptr<Pattern>> m_patterns;
            
            std::shared_ptr<EnemyNode> m_selected;
            bool m_onEditMode;
            
            const sf::Color DEFAULT_TEXT_COLOR = sf::Color(255, 255, 255, 150);
    };
}

