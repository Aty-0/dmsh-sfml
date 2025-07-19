#pragma once 
#include "../core/sceneManager.h"
#include "../core/component.h"
#include "../core/inputManager.h"
#include "enemyNode.h"
#include <vector>

namespace dmsh::game
{    
    // TODO: Patterns
    class Line
    {
        public:
            static void drawLine(sf::RenderWindow& window, const sf::Vector2f& first, const sf::Vector2f& end, const sf::Color& color) 
            {
                const sf::Vertex line[] =
                {
                    { first, color, { 0.0f , 0.0f } }, 
                    { end, color, { 0.0f , 0.0f } } 
                };

                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
    };

    class NodeEditor : public core::Component
    {
        public:
            NodeEditor()
            {
                const auto inputManager = core::InputManager::getInstance();
                inputManager->addListener(core::InputListenerType::KeyPressed, core::KeyCode::Z, [this]() {
                    m_onEditMode = !m_onEditMode;
                });
            }   
            
            virtual void onRender(sf::RenderWindow& window) override
            {
                std::shared_ptr<EnemyNode> prev = nullptr; 

                for (auto node : m_nodes)
                {
                    if (prev != nullptr)
                    {
                        auto currentPos = node->getOwner()->getTransform().getPosition();
                        auto prevPos = prev->getOwner()->getTransform().getPosition();

                        Line::drawLine(window, currentPos, prevPos, sf::Color::White);
                    }

                    prev = node;
                }
                
            }

            virtual void onMouseClicked(const sf::Vector2f& pos) override
            {            
                if (m_onEditMode)
                    return;

                const static auto sceneManager = core::SceneManager::getInstance();
                auto go = sceneManager->createGameObject<core::GameObject>();
                
                auto collider = go->createComponent<core::RectangleCollider>();
                collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 30.0f, 30.0f } });
                
                auto& goDrawable = go->getDrawable().createDrawable<sf::RectangleShape>();
                goDrawable.setSize({30, 30});
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