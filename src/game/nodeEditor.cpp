#include "nodeEditor.h"
#include "stageLoader.h"

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
                    deleteCurrentPattern(); 

                    if (m_currentPattern == nullptr)
                    {
                        ImGui::End();
                        return;
                    }
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
                ImGui::Text(m_editorMode == EditorMode::Edit ? "Edit mode" : "Creation mode");
                ImGui::Separator();
                auto& nodes = m_currentPattern->Nodes;
                std::shared_ptr<EnemyNode> prevNode = nullptr;
                for (std::int32_t i = 0; i < nodes.size(); ++i)
                {
                    auto node = nodes[i];
                    if (!node)
                    {
                        continue;
                    }

                    const auto name = std::format("Node: {}", i);

                    if (ImGui::Selectable(name.data(), node->m_isSelected))
                    {
                        if (prevNode && prevNode != node)
                            prevNode->m_isSelected = false;
                            
                        node->m_isSelected = !node->m_isSelected;
                        node->onIsSelectedChanged();
                        prevNode = node;
                    }
                }            
                
                ImGui::Separator();
                if (m_selected == nullptr)
                {
                    ImGui::Text("Selected:None");
                }
                else 
                {
                    const auto index = static_cast<std::size_t>(std::find(nodes.begin(), nodes.end(), m_selected) - nodes.begin());
                    const auto transform = m_selected->getOwner()->getTransform();
                    auto pos = transform->getPosition();
                    ImGui::Text("Selected:%i", index);

                    if (ImGui::InputFloat2("Position", &pos.x)) 
                    {
                        transform->setPosition(pos);
                    }

                    if (ImGui::Checkbox("Use Spline", &m_selected->m_useSpline))
                    {
                        m_selected->m_useSpline = !m_selected->m_useSpline;   
                    }

                    if (ImGui::Checkbox("Loop", &m_selected->m_loop))
                    {
                        m_selected->m_loop = !m_selected->m_loop;
                    }

                    ImGui::Text("TODO: Run event list");
                    if (ImGui::Checkbox("Shooting on this node", &m_selected->m_shoot))
                    {
                        m_selected->m_shoot = !m_selected->m_shoot;
                    }

                    ImGui::Text("TODO: Shoot Timer");
                    ImGui::Text("TODO: Bullet Type");
                    ImGui::Text("TODO: Run onDie event list");
                }

                ImGui::Separator();


            }
            else 
            {
                if (ImGui::Button("Create Pattern"))
                {                    
                    createNewPattern();
                }

                ImGui::Text("No patterns");
            }

            // FIXME: Ugly
            ImGui::Separator();
            static std::vector<char> saveFileNamebuffer(255);
            ImGui::InputText("File name##Save", saveFileNamebuffer.data(), saveFileNamebuffer.size());

            if (ImGui::Button("Save"))
            {
                save(std::string_view(saveFileNamebuffer.data(), std::strlen(saveFileNamebuffer.data())));
            }
            // FIXME: Ugly
            ImGui::Separator();
            static std::vector<char> loadFileNamebuffer(255);
            ImGui::InputText("File name##Load", loadFileNamebuffer.data(), loadFileNamebuffer.size());

            if (ImGui::Button("Load"))
            {
                const static auto stageLoader = StageLoader::getInstance();
                stageLoader->loadPattern(std::string_view(loadFileNamebuffer.data(), std::strlen(loadFileNamebuffer.data())));
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
        
        if (m_editorMode == EditorMode::Edit && m_selected->m_isSelected)
        {
            static const auto gameWindow = core::Window::getInstance();
            const auto mousePos = gameWindow->getMousePosition();
            const auto worldCoords = gameWindow->getWindow().mapPixelToCoords(mousePos);
            m_selected->getOwner()->getTransform()->setPosition(worldCoords);
        }
    }
    
    void NodeEditor::deleteNode(std::shared_ptr<EnemyNode> node)
    {
        if (!node)
        {
            DMSH_ERROR("Trying to delete null node!");
            return;
        }

        const static auto sceneManager = core::SceneManager::getInstance();
        auto owner = node->getOwner();  
        auto index = m_currentPatternIndex;
        if (index > 0)
            index--;              
        auto& nodes = m_patterns[index]->Nodes;
        std::erase(nodes, node);

        sceneManager->deleteGameObject(owner);
    }
            
    void NodeEditor::onInput(core::InputManager& input) 
    {
        if (input.isListenerActive("editor_delete_node"))
        {
            if (m_selected)
            {
                deleteNode(m_selected);
                m_selected = nullptr; 
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

        m_editorMode = !input.isListenerActive("editor_switch_edit_mode") ? EditorMode::Edit : EditorMode::Creation;
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
                core::debug::Debug<core::debug::DebugRenderType::Arrow>::draw(window, prevPos, currentPos, sf::Color::White);
            }
            prev = node;
        }     
    }
    
    void NodeEditor::createNodeByJsonNode(JsonNode node)
    {
        const auto gameNode = createNode();
        const auto transform = gameNode->getOwner()->getTransform();
        transform->setPosition({node.x, node.y});

        // TODO: Other params
    }

    std::shared_ptr<EnemyNode> NodeEditor::createNode()
    {
        DMSH_ASSERT(m_currentPattern, "No pattern selected but we are trying to create new node");

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
        
        const auto node = go->createComponent<EnemyNode>();
        node->m_nodeEditor = this;
        
        nodes.push_back(node);
        sceneManager->rebuildZOrdering();
        
        return node;
    }

    void NodeEditor::onMouseClicked(const sf::Vector2f& pos)
    {            
        if (m_editorMode != EditorMode::Creation || m_currentPattern == nullptr)
            return;
        
        const auto node = createNode();
        const auto transform = node->getOwner()->getTransform();
        transform->setPosition(pos);
    }

    void NodeEditor::deleteCurrentPattern()
    {
        if (m_patterns.size() == 0)
            return;

        m_selected = nullptr;

        const auto nodes = m_currentPattern->Nodes;
        for (auto node : nodes)
        {
            deleteNode(node);
        }   

        std::erase(m_patterns, m_currentPattern);
        if (m_patterns.size() > 0)
        {
            if (m_currentPatternIndex != 0)
                m_currentPatternIndex--;
    
            m_currentPattern = m_patterns[m_currentPatternIndex];
            auto& nodes = m_currentPattern->Nodes;
            setVisibilityNodes(nodes, true);
        }
        else
        {
            m_currentPattern = nullptr;
        }
    }

    void NodeEditor::clear()
    {        
        if (m_patterns.size() == 0)
            return;
        
        m_selected = nullptr;

        for (auto pattern : m_patterns)
        {
            const auto nodes = pattern->Nodes;
            for (auto node : nodes)
            {
                deleteNode(node);
            }            
        }
        
        m_currentPattern = nullptr;
        m_currentPatternIndex = 0;

        m_patterns.clear();
    }

    void NodeEditor::createNewPattern()
    {
        if (m_currentPattern)
        {
            // Turn off nodes in prev pattern 
            setVisibilityNodes(m_currentPattern->Nodes, false);
        }

        m_currentPattern = std::make_shared<Pattern>();
        if (m_patterns.size() > 0)
            m_currentPatternIndex++;
        m_patterns.push_back(m_currentPattern);
    }
    
    void NodeEditor::save(std::string_view name)
    {
        if (name.size() == 0)
        {
            return;
        }

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
                }
                
                patterns.push_back(JsonPattern {
                    .nodes = jsonNodes
                });
            }
            
            nlohmann::json patternJsonStructure = { "patterns", patterns };
            resourceManager->save(std::format("patterns/{}.json", name), patternJsonStructure.dump(4).data());
        }
    }
}