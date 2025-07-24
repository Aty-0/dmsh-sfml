#include "nodeEditor.h"
#include "../core/inputManager.h"
#include "../core/debug.h"
#include "../core/window.h"

namespace dmsh::game
{
    void NodeEditor::onStart()
    {       
        const auto inputManager = core::InputManager::getInstance();
        inputManager->addListener("editor_create_new_pattern", core::InputListenerType::KeyPressed, core::KeyCode::C);
        inputManager->addListener("editor_move_node", core::InputListenerType::KeyHold, core::MouseButtons::Left);
        inputManager->addListener("editor_switch_pattern_next", core::InputListenerType::KeyPressed, core::KeyCode::T);
        inputManager->addListener("editor_switch_pattern_back", core::InputListenerType::KeyPressed, core::KeyCode::R);
        inputManager->addListener("editor_delete_node", core::InputListenerType::KeyPressed, core::KeyCode::Delete);
        inputManager->addListener("editor_create_node", core::InputListenerType::KeyPressed, core::KeyCode::L);
        inputManager->addListener("editor_test", core::InputListenerType::KeyHold, core::KeyCode::H);

        createUI();        
        updateSelectedText();
        
        createNewPattern();
    }

    void NodeEditor::createUI()
    {
        const auto sceneManager = core::SceneManager::getInstance();
        auto selectedObjectTextGo = sceneManager->createGameObject<core::GameObject>();        
        m_selectedObjectTextComp = selectedObjectTextGo->createComponent<core::Text>();
        m_selectedObjectTextComp->setFillColor(DEFAULT_TEXT_COLOR);
        m_selectedObjectTextComp->setSize(16);
        auto selectedObjectTextTransform = selectedObjectTextGo->getTransform();
        selectedObjectTextTransform.setPosition({0, 100});
        
        auto patternTextGo = sceneManager->createGameObject<core::GameObject>();        
        m_patternTextComp = patternTextGo->createComponent<core::Text>();
        m_patternTextComp->setFillColor(DEFAULT_TEXT_COLOR);
        m_patternTextComp->setSize(16);
        auto patternTextTransform = patternTextGo->getTransform();
        patternTextTransform.setPosition({0, 50});

        auto editModeTextGo = sceneManager->createGameObject<core::GameObject>();        
        auto editModeTextComp = editModeTextGo->createComponent<core::Text>();
        editModeTextComp->setFillColor(DEFAULT_TEXT_COLOR);
        editModeTextComp->setSize(16);
        editModeTextComp->setText(m_onEditMode ? "Edit mode" : "Creation mode");
        
        auto editModeTextTransform = editModeTextGo->getTransform();
        editModeTextTransform.setPosition({0, 150});         

        auto noteTextGo = sceneManager->createGameObject<core::GameObject>();        
        auto noteTextComp = noteTextGo->createComponent<core::Text>();
        noteTextComp->setFillColor(DEFAULT_TEXT_COLOR);
        noteTextComp->setSize(16);
        noteTextComp->setText("Note:\nCreate pattern: key - C\nNext Pattern: key - T\nPrevious pattern: key - R\nSwitch edit mode: key - Z");
        
        auto noteTextTransform = noteTextGo->getTransform();
        noteTextTransform.setPosition({0, 550});   

        const auto inputManager = core::InputManager::getInstance();
        inputManager->addListener("editor_switch_edit_mode", core::InputListenerType::KeyPressed, core::KeyCode::Z, [this, editModeTextComp]() {
            m_onEditMode = !m_onEditMode;
            editModeTextComp->setText(m_onEditMode ? "Edit mode" : "Creation mode");
        });
    }
    
    void NodeEditor::setVisibilityNodes(std::vector<std::shared_ptr<EnemyNode>>& nodes, bool visible)
    {
        for (auto node : nodes)
        {
            node->getOwner()->setVisible(visible);
        }
    }

    void NodeEditor::switchToNextPattern()
    {
        if (!((m_currentPatternIndex + 1) >= m_patterns.size()))
        {
            m_currentPatternIndex++;

            // Turn off nodes in prev pattern 
            setVisibilityNodes(m_currentPattern->Nodes, false);
            // Set new pattern
            m_currentPattern = m_patterns[m_currentPatternIndex];
            // Turn on visible for current 
            setVisibilityNodes(m_currentPattern->Nodes, true);

            updatePatternText();
        }
    }

    void NodeEditor::switchToBackPattern()
    {
        if (m_currentPatternIndex > 0 && m_patterns.size() != 0)
        {
            m_currentPatternIndex--;

            // Turn off nodes in prev pattern 
            setVisibilityNodes(m_currentPattern->Nodes, false);
            // Set new pattern
            m_currentPattern = m_patterns[m_currentPatternIndex];
            // Turn on visible for current 
            setVisibilityNodes(m_currentPattern->Nodes, true);

            updatePatternText();
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
            m_selected->getOwner()->getTransform().setPosition(worldCoords);
        }
    }

    void NodeEditor::onInput(core::InputManager& input) 
    {
        if (input.isListenerActive("editor_test"))
        {
            DMSH_DEBUG("test");
        }

        if (input.isListenerActive("editor_delete_node"))
        {
            if (m_selected)
            {
                const static auto sceneManager = core::SceneManager::getInstance();
                auto owner = m_selected->getOwner();                
                std::erase(m_patterns[m_currentPatternIndex - 1]->Nodes, m_selected);
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
    }

    void NodeEditor::onRender(sf::RenderWindow& window)
    {                
        if (m_currentPattern == nullptr)
            return;

        auto nodes = m_currentPattern->Nodes;
        if (nodes.size() < 1)
            return;

        std::shared_ptr<EnemyNode> prev = nullptr; 
        for (auto node : nodes)
        {
            if (node == nullptr) 
            {
                continue;
            }
            if (prev != nullptr)
            {
                auto currentPos = node->getOwner()->getTransform().getPosition();
                auto prevPos = prev->getOwner()->getTransform().getPosition();
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
        
        auto& goDrawable = go->getDrawable().create<sf::RectangleShape>();
        goDrawable.setSize(size);
        goDrawable.setFillColor(sf::Color::White);
        auto& transform = go->getTransform();
        transform.setPosition(pos);
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
        updatePatternText();
        m_currentPatternIndex++;
    }
    
    void NodeEditor::updatePatternText()
    {
        if (m_currentPattern == nullptr)
        {
            m_patternTextComp->setText("No pattern selected or created!");
        }
        else 
        {
            m_patternTextComp->setText("Pattern:{}", m_currentPatternIndex);
        }
    }
    
    void NodeEditor::updateSelectedText()
    {
        if (m_selected == nullptr || m_currentPattern == nullptr)
        {
            m_selectedObjectTextComp->setText("Selected:None");
        }
        else
        {
            auto nodes = m_currentPattern->Nodes;
            const auto index = static_cast<std::size_t>(std::find(nodes.begin(), nodes.end(), m_selected) - nodes.begin());
            const auto transform = m_selected->getOwner()->getTransform();
            const auto pos = transform.getPosition();
                             
            m_selectedObjectTextComp->setText("Selected:{}\nPos [x:{} y:{}]", index, pos.x, pos.y);
        }
    }
}