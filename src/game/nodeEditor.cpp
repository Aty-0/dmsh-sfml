#include "nodeEditor.h"
#include "../core/inputManager.h"
#include "../core/debug.h"
#include "../core/window.h"

namespace dmsh::game
{   
    static const auto resourceManager = core::ResourceManager::getInstance();          

    void NodeEditor::onGui()
    {
        ImGui::Begin("Node editor");
        {
            if (m_currentPattern)
            {
                ImGui::Text("Pattern %i", m_currentPatternIndex);
                ImGui::Separator();

                if (ImGui::Button("Prev Pattern"))
                {
                    switchToBackPattern();
                }

                ImGui::SameLine();
                if (ImGui::Button("Delete Pattern"))
                {
                    // TODO: 
                }

                ImGui::SameLine();
                if (ImGui::Button("Create Pattern"))
                {                    
                    createNewPattern();
                }

                ImGui::SameLine();
                if (ImGui::Button("Next Pattern"))
                {
                    switchToNextPattern();                    
                }

                ImGui::Separator();
                ImGui::Text(m_onEditMode ? "Edit mode" : "Creation mode");
                ImGui::Separator();
                auto nodes = m_currentPattern->Nodes;
                if (m_selected == nullptr || m_currentPattern == nullptr)
                {
                    ImGui::Text("Selected:None");
                }
                else 
                {
                    const auto index = static_cast<std::size_t>(std::find(nodes.begin(), nodes.end(), m_selected) - nodes.begin());
                    const auto transform = m_selected->getOwner()->getTransform();
                    const auto pos = transform->getPosition();
                    ImGui::Text("Selected:%i\nPos [x:%f y:%f]", index, pos.x, pos.y);
                }

                ImGui::Separator();
                for (std::int32_t i = 0; i < nodes.size(); ++i)
                {
                    auto node = nodes[i];
                    if (node)
                    {
                        ImGui::Text("Node: %i", i);
                    }
                }            
            }
            else 
            {
                ImGui::Text("No pattern");
            }

            ImGui::Separator();
            static std::vector<char> buffer(255);
            ImGui::InputText("File name", buffer.data(), buffer.size());

            if (ImGui::Button("Save"))
            {
                save(std::string_view(buffer.data(), std::strlen(buffer.data())));
            }
            ImGui::TextDisabled("Note:\nCreate pattern: key - C\nNext Pattern: key - T\nPrevious pattern: key - R\nHold shift to switch to create mode.");
        }
        ImGui::End();
    }

    void NodeEditor::onStart()
    {       
        const auto inputManager = core::InputManager::getInstance();
        inputManager->addListener("editor_create_new_pattern", core::InputListenerType::KeyPressed, core::KeyCode::C);
        inputManager->addListener("editor_move_node", core::InputListenerType::KeyHold, core::MouseButtons::Left);
        inputManager->addListener("editor_switch_pattern_next", core::InputListenerType::KeyPressed, core::KeyCode::T);
        inputManager->addListener("editor_switch_pattern_back", core::InputListenerType::KeyPressed, core::KeyCode::R);
        inputManager->addListener("editor_delete_node", core::InputListenerType::KeyPressed, core::KeyCode::Delete);
        inputManager->addListener("editor_create_node", core::InputListenerType::KeyPressed, core::KeyCode::L);
        inputManager->addListener("editor_switch_edit_mode", core::InputListenerType::KeyHold, core::KeyCode::LShift);
      
        createNewPattern();
    }

    void NodeEditor::setVisibilityNodes(std::vector<std::shared_ptr<EnemyNode>>& nodes, bool visible)
    {
        for (std::int32_t i = 0; i < nodes.size(); ++i)
        {
            auto node = nodes[i];
            if (node)
                node->getOwner()->setVisible(visible);
        }
    }

    void NodeEditor::switchPattern()
    {
        // Turn off nodes in prev pattern 
        setVisibilityNodes(m_currentPattern->Nodes, false);
        // Set new pattern
        m_currentPattern = m_patterns[m_currentPatternIndex];
        // Turn on visible for current 
        setVisibilityNodes(m_currentPattern->Nodes, true);
    }

    void NodeEditor::switchToNextPattern()
    {
        if (!((m_currentPatternIndex + 1) >= m_patterns.size()))
        {
            m_currentPatternIndex++;
            switchPattern();
        }
    }

    void NodeEditor::switchToBackPattern()
    {
        if (m_currentPatternIndex > 0 && m_patterns.size() != 0)
        {
            m_currentPatternIndex--;
            switchPattern();
        }
    }

    void NodeEditor::moveNode()
    {
        if (!m_selected)
            return;
        
        if (m_onEditMode && m_selected->m_isSelected)
        {
            static const auto gameWindow = core::Window::getInstance();
            const auto mousePos = gameWindow->getMousePosition();
            const auto worldCoords = gameWindow->getWindow().mapPixelToCoords(mousePos);
            m_selected->getOwner()->getTransform()->setPosition(worldCoords);
        }
    }

    void NodeEditor::onInput(core::InputManager& input) 
    {
        if (input.isListenerActive("editor_delete_node"))
        {
            if (m_selected)
            {
                const static auto sceneManager = core::SceneManager::getInstance();
                auto owner = m_selected->getOwner();  
                auto index = m_currentPatternIndex;
                if (index > 0)
                    index--;              

                std::erase(m_patterns[index]->Nodes, m_selected);
                m_selected = nullptr; 
                sceneManager->deleteGameObject(owner);
            }
        }

        if (input.isListenerActive("editor_create_new_pattern"))
        {
            createNewPattern();
        }

        if (input.isListenerActive("editor_move_node"))
        {
            moveNode();
        }

        if (input.isListenerActive("editor_switch_pattern_next"))
        {
            switchToNextPattern();
        }
        
        if (input.isListenerActive("editor_switch_pattern_back"))
        {
            switchToBackPattern();
        }

        m_onEditMode = !input.isListenerActive("editor_switch_edit_mode");
    }

    void NodeEditor::onRender(sf::RenderTarget& window)
    {                
        if (m_currentPattern == nullptr)
            return;

        auto nodes = m_currentPattern->Nodes;
        if (nodes.size() < 1)
            return;

        std::shared_ptr<EnemyNode> prev = nullptr; 
        for (std::int32_t i = 0; i < nodes.size(); ++i)
        {
            auto node = nodes[i];
            if (node == nullptr) 
            {
                continue;
            }

            if (prev != nullptr)
            {
                auto currentPos = node->getOwner()->getTransform()->getPosition();
                auto prevPos = prev->getOwner()->getTransform()->getPosition();
                core::debug::Line::draw(window, currentPos, prevPos, sf::Color::White);
            }
            prev = node;
        }     
    }

    void NodeEditor::onMouseClicked(const sf::Vector2f& pos)
    {            
        if (m_onEditMode || m_currentPattern == nullptr)
            return;
        auto& nodes = m_currentPattern->Nodes;
        static const auto sceneManager = core::SceneManager::getInstance();
        auto go = sceneManager->createGameObject<core::GameObject>();
        
        auto collider = go->createComponent<core::RectangleCollider>();
        constexpr sf::Vector2f size = { 30.0f, 30.0f };
        collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, size });
        collider->setStatic(true);
        
        const auto nodeTexture = resourceManager->get<core::ResourceTypes::Texture>("node");
        DMSH_ASSERT(nodeTexture, "node texture is invalid");        
        auto& goDrawable = go->getDrawable()->create<sf::Sprite>(*nodeTexture->getHandle());        

        auto transform = go->getTransform();
        transform->setPosition(pos);
        
        auto node = go->createComponent<EnemyNode>();
        node->m_nodeEditor = this;
        
        nodes.push_back(node);
        sceneManager->rebuildZOrdering();
    }

    void NodeEditor::createNewPattern()
    {
        if (m_currentPattern)
        {
            // Turn off nodes in prev pattern 
            setVisibilityNodes(m_currentPattern->Nodes, false);
        }

        m_currentPattern = std::make_shared<Pattern>();
        m_patterns.push_back(m_currentPattern);
        m_currentPatternIndex++;
    }
    
    void NodeEditor::save(std::string_view name)
    {
        std::vector<JsonPattern> patterns;
        for (std::int32_t i = 0; i < m_patterns.size(); ++i)
        {
            const auto pattern = m_patterns[i];
            if (pattern)
            {
                std::vector<JsonNode> jsonNodes;
                for (std::int32_t j = 0; j < pattern->Nodes.size(); ++j)
                {
                    const auto node = pattern->Nodes[j];
                    if (node)
                    {
                        const auto position = node->getOwner()->getTransform()->getPosition();
                        JsonNode jsonNode {
                           .isSelected = node->m_isSelected,
                           .useSpline = node->m_useSpline,
                           .isFirstNode = node->m_isFirstNode,
                           .isEndNode = node->m_isEndNode,
                           .x = position.x,
                           .y = position.y
                        };
                        jsonNodes.push_back(jsonNode);
                    }
                
                    patterns.push_back(JsonPattern {
                        .nodes = jsonNodes
                    });
                }
            }
            
            nlohmann::json patternJsonStructure = patterns;
            resourceManager->save(std::format("patterns/{}.json", name), patternJsonStructure.dump(4).data());
        }
    }
}