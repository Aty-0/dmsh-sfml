#pragma once 
#include "../core/sceneManager.h"
#include "../core/component.h"
#include "../core/inputManager.h"
#include "../core/debug.h"
#include "../core/window.h"

#include "enemyNode.h"
#include <vector>

namespace dmsh::game
{    
    // TODO: Patterns
    class NodeEditor : public core::Component
    {
        public:
            NodeEditor()
            {
                const auto inputManager = core::InputManager::getInstance();

                inputManager->addListener("editor_switch_edit_mode", core::InputListenerType::KeyPressed, core::KeyCode::Z, [this]() {
                    m_onEditMode = !m_onEditMode;
                });
                
                inputManager->addListener("editor_move_node", core::InputListenerType::KeyHold, core::MouseButtons::Left, [this]() {        
                    if (!m_selected)
                        return;

                    if (m_onEditMode && m_selected->m_isSelected)
                    {
                        static const auto gameWindow = core::Window::getInstance();
                        const auto mousePos = gameWindow->getMousePosition();
                        const auto worldCoords = gameWindow->getWindow().mapPixelToCoords(mousePos);
                        m_selected->getOwner()->getTransform().setPosition(worldCoords);
                    }
                });

            }   
            
            virtual void onRender(sf::RenderWindow& window) override
            {
                if (m_nodes.size() < 1)
                    return;

                std::shared_ptr<EnemyNode> prev = nullptr; 
                for (auto node : m_nodes)
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

            virtual void onMouseClicked(const sf::Vector2f& pos) override
            {            
                if (m_onEditMode)
                    return;

                static const auto sceneManager = core::SceneManager::getInstance();
                auto go = sceneManager->createGameObject<core::GameObject>();
                
                auto collider = go->createComponent<core::RectangleCollider>();

                constexpr sf::Vector2f size = { 30.0f, 30.0f };
                collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, size });
                
                auto& goDrawable = go->getDrawable().createDrawable<sf::RectangleShape>();
                goDrawable.setSize(size);
                goDrawable.setFillColor(sf::Color::White);

                auto& transform = go->getTransform();
                transform.setPosition(pos);

                auto node = go->createComponent<EnemyNode>();
                node->m_nodeEditor = this;
                
                m_nodes.push_back(node);

                sceneManager->rebuildZOrdering();
            }

            inline bool isOnEditMode() const { return m_onEditMode; }
            inline std::shared_ptr<EnemyNode> getSelected() const { return m_selected; }
            inline void setSelected(std::shared_ptr<EnemyNode> node) { m_selected = node; }
        private:
            std::shared_ptr<EnemyNode> m_selected;
            std::vector<std::shared_ptr<EnemyNode>> m_nodes;
            bool m_onEditMode;
    };
}