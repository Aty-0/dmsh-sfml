#pragma once
#include "../core/common.h"
#include "../core/gameObject.h"
#include "../core/inputManager.h"

namespace dmsh::game
{
    // TODO: component based
    struct Player : public core::GameObject
    {
        public:
            Player() : m_speed(500.5f), m_move({0,0})
            {
            }

            virtual void onStart() override 
            {
                getTransform().setPosition({500, 500});
                auto& shape = getDrawable().createDrawable<sf::RectangleShape>();
                shape.setSize({30, 50});
                shape.setFillColor(sf::Color::White);
                const auto inputManager = core::InputManager::getInstance();
    
                inputManager->addListener(core::InputListenerType::KeyHold, core::KeyCode::A, [this]() {
                    m_move.x = -1;
                });
    
                inputManager->addListener(core::InputListenerType::KeyHold, core::KeyCode::D, [this]() {
                    m_move.x = 1;
                });
    
                inputManager->addListener(core::InputListenerType::KeyHold, core::KeyCode::W, [this]() {
                    m_move.y = -1;
                });
    
                inputManager->addListener(core::InputListenerType::KeyHold, core::KeyCode::S, [this]() {
                    m_move.y = 1;
                });
                
                auto collider = createComponent<core::RectangleCollider>();
                collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 30.0f, 50.0f } });      
                
                setTag("player");
            }

            virtual void onUpdate(float delta) override
            {
                auto& transform = getTransform();
                const auto pos = transform.getPosition();
                transform.setPosition({pos.x + m_move.x * m_speed * delta, pos.y + m_move.y * m_speed * delta});
                m_move = { 0, 0 };
            }

        private:
            float m_speed;
            sf::Vector2i m_move;
    };
}
