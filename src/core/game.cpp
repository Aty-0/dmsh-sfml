#include "game.h"
#include "window.h"
#include "logger.h"
#include "time.h"
#include "sceneManager.h"
#include "inputManager.h"
#include "text.h"
#include "coroutine.h"

#include "../game/player.h"
#include "../game/nodeEditor.h"
#include "../game/enemy.h"

namespace dmsh::core
{
    static const auto sceneManager = SceneManager::getInstance(); 
    static const auto inputManager = InputManager::getInstance(); 
    static const auto coroutineScheduler = coroutines::CoroutineScheduler::getInstance(); 

    static const auto time = Time::getInstance();
    
    // TODO: Relocate to stats class 
    static bool showDebugInfo = false;
    
    static std::shared_ptr<GameObject> sceneDebugText;
    static std::shared_ptr<GameObject> fpsDebugText;    
    static std::shared_ptr<GameObject> inputDebugText;

    static std::shared_ptr<Text> sceneDebugTextComp;
    static std::shared_ptr<Text> fpsDebugTextComp;    
    static std::shared_ptr<Text> inputDebugTextComp;
    
    void Game::run()
    {
        const auto window = Window::getInstance();
        window->create(1280, 768, "dmsh-sfml");
        
        auto& sfWindow = window->getWindow();                
        
        Scene scene = { };
        sceneManager->set(scene);
        {            
            sceneDebugText = sceneManager->createGameObject<GameObject>();
            sceneDebugText->setVisible(false);

            sceneDebugTextComp = sceneDebugText->createComponent<Text>();
            sceneDebugTextComp->setFillColor(sf::Color::Yellow);
            sceneDebugTextComp->setSize(16);

            auto sceneDebugTextTransform = sceneDebugText->getTransform();
            sceneDebugTextTransform.setPosition({100, 100});

            fpsDebugText = sceneManager->createGameObject<GameObject>();            
            fpsDebugTextComp = fpsDebugText->createComponent<Text>();
            fpsDebugTextComp->setFillColor(sf::Color::Yellow);
            fpsDebugTextComp->setSize(18);

            
            inputDebugText = sceneManager->createGameObject<GameObject>();            
            inputDebugText->setVisible(false);
            inputDebugTextComp = inputDebugText->createComponent<Text>();
            inputDebugTextComp->setFillColor(sf::Color::Red);
            inputDebugTextComp->setSize(14);

            auto inputDebugTextTransform = inputDebugText->getTransform();
            inputDebugTextTransform.setPosition({500, 100});
            
            auto nodeEditor = sceneManager->createGameObject<GameObject>();
            nodeEditor->createComponent<game::NodeEditor>();

            auto playerGo = sceneManager->createGameObject<GameObject>();
            playerGo->createComponent<game::Player>();
            
            for (std::int32_t i = 0; i < 15; i++)
            {
                auto npc = sceneManager->createGameObject<GameObject>();
                auto& transform = npc->getTransform();
                npc->createComponent<game::Enemy>();
                transform.setPosition({transform.getPosition().x + (i * 5), transform.getPosition().y + (i * 10)});
            }
      
        }

        sceneManager->rebuildZOrdering();

        // Add listener for debug switcher
        inputManager->addListener("engine_debug_info_switch", core::InputListenerType::KeyPressed, core::KeyCode::F10, [&]() {
            showDebugInfo = !showDebugInfo;

            inputDebugText->setVisible(showDebugInfo);
            sceneDebugText->setVisible(showDebugInfo);
        });

        // Add listener for mouse select event for all game objects
        inputManager->addListener("engine_scene_on_mouse_clicked", core::InputListenerType::KeyPressed, core::MouseButtons::Left, [&]() {
            sceneManager->onMouseClicked(sfWindow);
        });

        inputManager->addListener("engine_show_colliders", core::InputListenerType::KeyPressed, core::KeyCode::F9, [&]() {
            auto scene = sceneManager->getScene();
            for (auto goWeak : scene.GameObjects)
            {
                auto go = goWeak.lock();
                if (go)
                {
                    auto rectCollider = go->getComponent<RectangleCollider>();
                    if (rectCollider)
                    {
                        rectCollider->setAlwaysShowRect(!rectCollider->getAlwaysShowRect());
                    }
                }
            }
        });

        while (window->isOpen())
        {
            poolEvents(sfWindow);
            sceneManager->onInput(*inputManager);
            onUpdate(time->getDelta());
            onRender(sfWindow);
        }
    }
    
    void Game::onRender(sf::RenderWindow& window)
    {
        window.clear();
        sceneManager->onRender(window);        
        window.display();
    }
    
    void Game::onUpdate(float delta)
    {
        inputManager->update();
        sceneManager->onUpdate(delta);
        coroutineScheduler->update();
        
        fpsDebugTextComp->setText("fps:{}\ndelta:{}", time->getFps(), delta);
        
        if (showDebugInfo)
        {
            inputDebugTextComp->setText(inputManager->toString());
            sceneDebugTextComp->setText(sceneManager->toString());
        }
    }
    
    void Game::poolEvents(sf::RenderWindow& window)
    {
        while(true)
        {
            const auto event = window.pollEvent();
            if (!event.has_value())
                break;
                
            const auto pureEvent = event.value();
            if (pureEvent.is<sf::Event::Closed>())
            {
                onClose();
                break;
            }

            inputManager->process(pureEvent);
        }
    }

    void Game::onClose()
    {
        DMSH_DEBUG("Close game");
        const auto window = Window::getInstance();        
        window->close();
    }

}