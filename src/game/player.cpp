#include "player.h"

namespace dmsh::game
{
    void Player::onStart()  
    {
        auto owner = getOwner();
        owner->getTransform().setPosition({500, 500});
        auto& shape = owner->getDrawable().create<sf::RectangleShape>();
        shape.setSize({30, 50});
        shape.setFillColor(sf::Color::White);
        const auto inputManager = core::InputManager::getInstance();
        inputManager->addListener("player_move_left", core::InputListenerType::KeyHold, core::KeyCode::Left);
        inputManager->addListener("player_move_right", core::InputListenerType::KeyHold, core::KeyCode::Right);
        inputManager->addListener("player_move_forward", core::InputListenerType::KeyHold, core::KeyCode::Up);
        inputManager->addListener("player_move_backward", core::InputListenerType::KeyHold, core::KeyCode::Down);
        inputManager->addListener("player_shoot", core::InputListenerType::KeyHold, core::KeyCode::A);
        
        auto collider = owner->createComponent<core::RectangleCollider>();
        collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 30.0f, 50.0f } });      
        
        owner->setTag("player");

        m_bulletPool = std::make_shared<BulletPool>(32);
    }
    
    void Player::onInput(core::InputManager& input)
    {
        if (input.isListenerActive("player_move_left"))
        {
            m_move.x = -1;
        }
        if (input.isListenerActive("player_move_right"))
        {
            m_move.x = 1;
        }
        if (input.isListenerActive("player_move_forward"))
        {
            m_move.y = -1;
        }
        if (input.isListenerActive("player_move_backward"))
        {
            m_move.y = 1;
        }
        if (input.isListenerActive("player_shoot"))
        {
            shoot();
        }
        else
        {
            stopShooting();
        }
    }
    
    void Player::onDestroy() 
    {
        const auto inputManager = core::InputManager::getInstance();
        
        inputManager->removeListener("player_move_left");
        inputManager->removeListener("player_move_right");
        inputManager->removeListener("player_move_forward");
        inputManager->removeListener("player_move_backward");
    }

    void Player::onUpdate(float delta) 
    {
        auto& transform = getOwner()->getTransform();
        const auto pos = transform.getPosition();
        transform.setPosition({pos.x + m_move.x * m_speed * delta, pos.y + m_move.y * m_speed * delta});
        m_move = { 0, 0 };
    }

    void Player::stopShooting()
    {
        static const auto coroutineScheduler = core::coroutines::CoroutineScheduler::getInstance(); 
        coroutineScheduler->stop(m_shootCoroutine);
        m_shootCoroutine = nullptr;
    }

    void Player::shoot()
    {
        static const auto coroutineScheduler = core::coroutines::CoroutineScheduler::getInstance(); 
        if (m_canShoot)
            m_shootCoroutine = coroutineScheduler->run(std::bind(&Player::shootCoroutine, this));
    }

    core::coroutines::Coroutine Player::shootCoroutine()
    {     
        m_bulletPool->pool(getOwner()->getTransform().getPosition());
        co_await core::coroutines::WaitForSeconds(m_shootDuration);                                          
    }
}