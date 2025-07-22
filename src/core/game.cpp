#include "game.h"
#include "window.h"
#include "logger.h"
#include "time.h"
#include "sceneManager.h"
#include "inputManager.h"
#include "text.h"

#include "../game/player.h"
#include "../game/nodeEditor.h"
#include "../game/enemy.h"

namespace dmsh::core
{
    static const auto sceneManager = SceneManager::getInstance(); 
    static const auto inputManager = InputManager::getInstance(); 
    static const auto time = Time::getInstance();
    
    // TODO: Relocate to stats class 
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
            auto sceneDebugText = sceneManager->createGameObject<GameObject>();
            
            sceneDebugTextComp = sceneDebugText->createComponent<Text>();
            sceneDebugTextComp->setFillColor(sf::Color::Yellow);
            sceneDebugTextComp->setSize(16);

            auto sceneDebugTextTransform = sceneDebugText->getTransform();
            sceneDebugTextTransform.setPosition({100, 100});

            auto fpsDebugText = sceneManager->createGameObject<GameObject>();            
            fpsDebugTextComp = fpsDebugText->createComponent<Text>();
            fpsDebugTextComp->setFillColor(sf::Color::Yellow);
            fpsDebugTextComp->setSize(18);

            
            auto inputDebugText = sceneManager->createGameObject<GameObject>();            
            inputDebugTextComp = inputDebugText->createComponent<Text>();
            inputDebugTextComp->setFillColor(sf::Color::Yellow);
            inputDebugTextComp->setSize(14);

            auto inputDebugTextTransform = inputDebugText->getTransform();
            inputDebugTextTransform.setPosition({500, 100});
            
            auto nodeEditor = sceneManager->createGameObject<GameObject>();
            nodeEditor->createComponent<game::NodeEditor>();

            auto player = sceneManager->createGameObject<game::Player>();

            for (std::int32_t i = 0; i < 15; i++)
            {
                auto npc = sceneManager->createGameObject<GameObject>();
                auto& transform = npc->getTransform();
                npc->createComponent<game::Enemy>();
                transform.setPosition({transform.getPosition().x + (i * 5), transform.getPosition().y + (i * 10)});
            }
      
        }

        sceneManager->rebuildZOrdering();
        
        // Add listener for mouse select for game objects
        inputManager->addListener("engine_scene_on_mouse_clicked", core::InputListenerType::KeyPressed, core::MouseButtons::Left, [&]() {
            sceneManager->onMouseClicked(sfWindow);
        });

        while (window->isOpen())
        {
            poolEvents(sfWindow);
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

        fpsDebugTextComp->setText("fps:{}\ndelta:{}", time->getFps(), delta);
        inputDebugTextComp->setText(inputManager->toString());
        sceneDebugTextComp->setText(sceneManager->toString());
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