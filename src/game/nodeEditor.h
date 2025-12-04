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
    class StageLoader;
    class NodeEditor : public core::Component
    {
        public:
            friend StageLoader;
            
            enum class EditorMode
            {
                Creation,
                Edit,
            };

            NodeEditor() : 
                m_currentPatternIndex(0), 
                m_editorMode(EditorMode::Edit)                
            {            
   
            }  
            
            virtual void onGui() override;
            virtual void onStart() override;
            virtual void onInput(core::InputManager& input) override;            
            virtual void onRender(sf::RenderTarget& window) override;            
            virtual void onMouseClicked(const sf::Vector2f& pos) override;

            inline EditorMode getEditorMode() const { return m_editorMode; }
            inline std::shared_ptr<EnemyNode> getSelected() const { return m_selected; }
            
            inline void setSelected(std::shared_ptr<EnemyNode> node) 
            {
                m_selected = node;
            }

            inline bool isWindowFocused() const { return m_windowFocus; }
        private:
            void createNodeByJsonNode(JsonNode jsonNode);
            std::shared_ptr<EnemyNode> createNode();
            void deleteNode(std::shared_ptr<EnemyNode> node);
            
            void clear();
            void save(std::string_view name);
            void createNewPattern(); 
            void deleteCurrentPattern();           
            void switchPattern();
            void switchToNextPattern();
            void switchToBackPattern();
            void moveNode();
            void setVisibilityNodes(std::vector<std::shared_ptr<EnemyNode>>& nodes, bool visible);

            std::int32_t m_currentPatternIndex;
            std::shared_ptr<Pattern> m_currentPattern;
            std::vector<std::shared_ptr<Pattern>> m_patterns;
            
            std::shared_ptr<EnemyNode> m_selected;
            EditorMode m_editorMode;
            bool m_windowFocus;

            const sf::Color DEFAULT_TEXT_COLOR = sf::Color(255, 255, 255, 150);
    };
}

