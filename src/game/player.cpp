#include "player.h"
#include "../core/resourceManager.h"

namespace dmsh::game
{
    static const auto resourceManager = core::ResourceManager::getInstance();

    // TODO: Need to think how we can avoid this, json ? 
    constexpr std::initializer_list<sf::IntRect> playerMoveLeftRectList = {
            { sf::IntRect { {0, 95}, {32, 47} } }, 
            { sf::IntRect { {32, 95 }, {32, 47} } }, 
            { sf::IntRect { {64, 95 }, {32, 47} } }, 
            { sf::IntRect { {96, 95 }, {32, 47} } }, 
            { sf::IntRect { {128, 95 }, {32, 47} } }, 
            { sf::IntRect { {160, 95 }, {32, 47} } }, 
            { sf::IntRect { {192, 95 }, {32, 47} } }, 
    };
    
    constexpr std::initializer_list<sf::IntRect> playerMoveRightRectList = {
            { sf::IntRect { {0, 47 }, {32, 47} } }, 
            { sf::IntRect { {32, 47 }, {32, 47} } }, 
            { sf::IntRect { {64, 47 }, {32, 47} } }, 
            { sf::IntRect { {96, 47 }, {32, 47} } }, 
            { sf::IntRect { {128, 47 }, {32, 47} } }, 
            { sf::IntRect { {160, 47 }, {32, 47} } }, 
            { sf::IntRect { {192, 47 }, {32, 47} } }, 
    };

    constexpr std::initializer_list<sf::IntRect> playerIdleRectList = {
            { sf::IntRect { {0, 0}, {32, 47} } }, 
            { sf::IntRect { {32, 0}, {32, 47} } }, 
            { sf::IntRect { {64, 0}, {32, 47} } }, 
            { sf::IntRect { {96, 0}, {32, 47} } }, 
            { sf::IntRect { {128, 0}, {32, 47} } }, 
            { sf::IntRect { {160, 0}, {32, 47} } }, 
            { sf::IntRect { {192, 0}, {32, 47} } }, 
    };

    // FIXME: oguzok
    void Player::setAnimation(const AnimationState& state)
    {
        static AnimationState currentState = AnimationState::None;
        if (currentState == state)
            return;

        currentState = state;
        switch (state)
        {
            case AnimationState::Idle:
                m_animatedTexture->setRectList(playerIdleRectList);
                break;
            case AnimationState::MoveLeft:
                m_animatedTexture->setRectList(playerMoveLeftRectList);
                break;
            case AnimationState::MoveRight:
                m_animatedTexture->setRectList(playerMoveRightRectList);
                break;
        }
    }

    void Player::onStart()  
    {
        auto owner = getOwner();
        const auto playerTexture = resourceManager->get<core::ResourceTypes::Texture>("player");
        DMSH_ASSERT(playerTexture, "player texture is invalid");        

        auto& shape = owner->getDrawable()->create<sf::Sprite>(*playerTexture->getHandle());
        shape.setTextureRect({{0, 0}, {30, 47}});

        const auto transform = owner->getTransform();
        transform->setPosition({500, 500});
        transform->setScale({1.3f, 1.3f});

        const auto inputManager = core::InputManager::getInstance();
        inputManager->addListener("player_move_left", core::InputListenerType::KeyHold, core::KeyCode::Left);
        inputManager->addListener("player_move_left", core::InputListenerType::KeyHold, core::KeyCode::Left);
        inputManager->addListener("player_move_right", core::InputListenerType::KeyHold, core::KeyCode::Right);
        inputManager->addListener("player_move_forward", core::InputListenerType::KeyHold, core::KeyCode::Up);
        inputManager->addListener("player_move_backward", core::InputListenerType::KeyHold, core::KeyCode::Down);
        inputManager->addListener("player_shoot", core::InputListenerType::KeyHold, core::KeyCode::A);
        
        auto collider = owner->createComponent<core::RectangleCollider>();
        collider->setRect(sf::FloatRect { { 0.0f, 0.0f }, { 30.0f, 50.0f } });      
        
        owner->setZDepth(100);
        owner->setTag("player");
    
        m_bulletPool = std::make_shared<BulletPool>(32);
        
        m_animatedTexture = owner->createComponent<core::AnimatedSpriteTexture>(shape, playerIdleRectList, 12.0f);
    }
    
    void Player::onInput(core::InputManager& input)
    {
        if (input.isListenerActive("player_move_left"))
        {
            m_move.x = -1;
            setAnimation(AnimationState::MoveLeft);
        }
        else if (input.isListenerActive("player_move_right"))
        {
            m_move.x = 1;
            setAnimation(AnimationState::MoveRight);
        }
        else 
        {
            setAnimation(AnimationState::Idle);
        }
        
        if (input.isListenerActive("player_move_forward"))
        {
            m_move.y = -1;
        }
        else if (input.isListenerActive("player_move_backward"))
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
        auto transform = getOwner()->getTransform();
        const auto pos = transform->getPosition();
        transform->setPosition({pos.x + m_move.x * m_speed * delta, pos.y + m_move.y * m_speed * delta});
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
            m_shootCoroutine = coroutineScheduler->run(std::move(std::bind(&Player::shootCoroutine, this)));
    }

    core::coroutines::Coroutine Player::shootCoroutine()
    {     
        m_bulletPool->pool(getOwner()->getTransform()->getPosition());
        co_await core::coroutines::WaitForSeconds(m_shootDuration);                                          
    }
}