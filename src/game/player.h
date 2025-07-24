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
                auto& shape = getDrawable().create<sf::RectangleShape>();
                shape.setSize({30, 50});
                shape.setFillColor(sf::Color::White);
                const auto inputManager = core::InputManager::getInstance();
    
                inputManager->addListener("player_move_left", core::InputListenerType::KeyHold, core::KeyCode::Left, [this]() {
                    m_move.x = -1;
                });
    
                inputManager->addListener("player_move_right", core::InputListenerType::KeyHold, core::KeyCode::Right, [this]() {
                    m_move.x = 1;
                });
    
                inputManager->addListener("player_move_forward", core::InputListenerType::KeyHold, core::KeyCode::Up, [this]() {
                    m_move.y = -1;
                });
    
                inputManager->addListener("player_move_backward", core::InputListenerType::KeyHold, core::KeyCode::Down, [this]() {
                    m_move.y = 1;
                });
                
                auto collider = createComponent<core::RectangleCollider>();
                collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 30.0f, 50.0f } });      
                
                setTag("player");
            }

            virtual void onDestroy() override
            {
                const auto inputManager = core::InputManager::getInstance();
                
                inputManager->removeListener("player_move_left");
                inputManager->removeListener("player_move_right");
                inputManager->removeListener("player_move_forward");
                inputManager->removeListener("player_move_backward");
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
