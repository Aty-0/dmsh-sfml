#include "game.h"
#include "window.h"
#include "logger.h"
#include "time.h"
#include "sceneManager.h"
#include "inputManager.h"
#include "text.h"
#include "coroutine.h"
#include "spatialGrid.h"
#include "view.h"
#include "resourceManager.h"
#include "postEffects.h"

#include "../game/player.h"
#include "../game/nodeEditor.h"
#include "../game/enemy.h"

#define USE_BENCHMARK

namespace dmsh::core
{
    template<typename Function, typename Owner, typename... Args>
    inline float runBenchmark(Function&& func, Owner* owner, Args&&... args)
    {
        static_assert(std::is_invocable<Function, Owner, Args...>::value, "is not a invocable function...");

        const auto start = std::chrono::high_resolution_clock::now();
        std::invoke(std::forward<Function>(func), owner, std::forward<Args>(args)...);
        const auto end = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration<float, std::milli>(end - start).count();
        return ms; 
    }

    static const auto postEffectManager = PostEffectManager::getInstance(); 
    static const auto soundManager = SoundManager::getInstance(); 
    static const auto sceneManager = SceneManager::getInstance(); 
    static const auto inputManager = InputManager::getInstance(); 
    static const auto coroutineScheduler = coroutines::CoroutineScheduler::getInstance(); 
    static const auto collisionGrid = CollisionSpatialGrid::getInstance(); 
    static const auto time = Time::getInstance();

#ifdef USE_BENCHMARK
    static float benchmarkRenderMs = 0.0f;
    static float benchmarkUpdateMs = 0.0f;
    static float benchmarkSoundsMs = 0.0f;
    static float benchmarkInputMs = 0.0f;
    static float benchmarkPoolEventsMs = 0.0f;
#endif
    
    // TODO: Relocate to stats class 
    static bool showDebugInfo = false;
    static bool showColliderGrid = false;
    
    static std::shared_ptr<GameObject> sceneDebugText;
    static std::shared_ptr<GameObject> fpsDebugText;    
    static std::shared_ptr<GameObject> inputDebugText;

    static std::shared_ptr<Text> sceneDebugTextComp;
    static std::shared_ptr<Text> fpsDebugTextComp;    
    static std::shared_ptr<Text> inputDebugTextComp;
    
    static std::shared_ptr<sf::Shader> grainShader;
    static std::shared_ptr<sf::Shader> chromaticAbShader;
    
    void Game::run()
    {
        DMSH_ASSERT(sf::Shader::isAvailable(), "Shaders are not available!");
        DMSH_ASSERT(sf::Shader::isGeometryAvailable(), "Geom shaders are not available!");
        const auto window = Window::getInstance();
        window->create(1280, 768, "dmsh-sfml");
        auto& sfWindow = window->getWindow();     
        DMSH_ASSERT(ImGui::SFML::Init(sfWindow), "Failed to init sfml-imgui");

        const auto resourceManager = ResourceManager::getInstance();
        resourceManager->init();
        resourceManager->load<ResourceTypes::Font>("fonts/immortal.ttf", "immortal");
        // TODO: Remove
        resourceManager->load<ResourceTypes::Sound>("sounds/damage00.wav", "damage");
        resourceManager->load<ResourceTypes::Texture>("textures/node.png", "node");
        resourceManager->load<ResourceTypes::Texture>("textures/player.png", "player");
        resourceManager->load<ResourceTypes::Texture>("textures/enemy_1.png", "enemy_1");
        resourceManager->load<ResourceTypes::Texture>("textures/bullet.png", "bullet");

        resourceManager->load<ResourceTypes::Shader>("shaders/grain.frag", "grain", sf::Shader::Type::Fragment);
        resourceManager->load<ResourceTypes::Shader>("shaders/pt.frag", "pt", sf::Shader::Type::Fragment);
        resourceManager->load<ResourceTypes::Shader>("shaders/ca.frag", "ca", sf::Shader::Type::Fragment);

        auto grainShaderResource = ResourceManager::getInstance()->get<ResourceTypes::Shader>("grain");
        DMSH_ASSERT(grainShaderResource, "Failed to load grain shader");
        grainShader = grainShaderResource->getHandle();
        
        auto caShaderResource = ResourceManager::getInstance()->get<ResourceTypes::Shader>("ca");
        DMSH_ASSERT(caShaderResource, "Failed to load chromatic aberration shader");
        chromaticAbShader = caShaderResource->getHandle();

        const auto windowSize = window->getSize();        
        postEffectManager->createCanvas(windowSize);
        postEffectManager->create(windowSize, grainShader);
        postEffectManager->create(windowSize, chromaticAbShader);

        // TODO: If we want to change window res, we need to update this       
        // TODO: What's size will be perfect ?? 
        const auto cellDefaultSize = sf::Vector2i(150, 150);
        const sf::Vector2i numCells = { 
            static_cast<std::int32_t>(floor(windowSize.x / cellDefaultSize.x)),
            static_cast<std::int32_t>(floor(windowSize.y / cellDefaultSize.y)) 
        };
        const sf::Vector2f sizeCells = {
            static_cast<float>(floor(windowSize.x / numCells.x)) + 2.0f,
            static_cast<float>(floor(windowSize.y / numCells.y)) + 2.0f 
        };
        
        collisionGrid->createCells(numCells, sizeCells);

        Scene scene = { };
        sceneManager->set(std::move(scene));
        {            
            sceneDebugText = sceneManager->createGameObject<GameObject>();
            sceneDebugText->setVisible(false);

            sceneDebugTextComp = sceneDebugText->createComponent<Text>();
            sceneDebugTextComp->setFillColor(sf::Color::Yellow);
            sceneDebugTextComp->setSize(16);

            auto sceneDebugTextTransform = sceneDebugText->getTransform();
            sceneDebugTextTransform->setPosition({100, 100});

            fpsDebugText = sceneManager->createGameObject<GameObject>();            
            fpsDebugTextComp = fpsDebugText->createComponent<Text>();
            fpsDebugTextComp->setFillColor(sf::Color::Yellow);
            fpsDebugTextComp->setSize(18);

            auto fpsDebugTextTransform = fpsDebugText->getTransform();
            fpsDebugTextTransform->setPosition({windowSize.x * 0.7f, 0});
          
            inputDebugText = sceneManager->createGameObject<GameObject>();            
            inputDebugText->setVisible(false);
            inputDebugTextComp = inputDebugText->createComponent<Text>();
            inputDebugTextComp->setFillColor(sf::Color::Red);
            inputDebugTextComp->setSize(14);

            auto inputDebugTextTransform = inputDebugText->getTransform();
            inputDebugTextTransform->setPosition({500, 100});
            
            auto nodeEditor = sceneManager->createGameObject<GameObject>();
            nodeEditor->createComponent<game::NodeEditor>();

            auto playerGo = sceneManager->createGameObject<GameObject>();
            playerGo->createComponent<game::Player>();
            
            for (std::int32_t i = 0; i < 10; ++i)
            {
                for (std::int32_t j = 0; j < 10; ++j)
                {
                    auto npc = sceneManager->createGameObject<GameObject>();
                    auto transform = npc->getTransform();
                    npc->createComponent<game::Enemy>();

                    transform->setPosition({100.0f + (j * 70), 100.0f + (i * 60)});
                }
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

        inputManager->addListener("engine_switch_posteffects", core::InputListenerType::KeyPressed, core::KeyCode::F8, [&]() {
            postEffectManager->setEnable(!postEffectManager->isEnable());
        });
        
        inputManager->addListener("engine_show_colliders", core::InputListenerType::KeyPressed, core::KeyCode::F9, [&]() {
            RectangleCollider::setAlwaysShowRect(!RectangleCollider::getAlwaysShowRect());
            showColliderGrid = !showColliderGrid;
        });

        auto& gameSpaceView = getViewSpaceGame();
        auto& view = gameSpaceView.getView();
        
        view.setCenter({ windowSize.x * 0.5f, windowSize.y * 0.5f });
        view.setSize(sf::Vector2f(windowSize));
        view.setScissor({{0.0f, 0.0f}, {0.7f, 1.0f}});
        
        auto& uiSpaceView = getViewSpaceUI();
        auto& uiView = uiSpaceView.getView();
        uiView.setSize(sf::Vector2f(windowSize));
        uiView.setCenter({ windowSize.x / 2.0f, windowSize.y / 2.0f });            
        
        runLoop(window);
        
        ImGui::SFML::Shutdown();
    }
    
    void Game::runLoop(Window* window)
    {
        auto& sfWindow = window->getWindow();     

        while (window->isOpen())
        {      
            time->update();
#ifdef USE_BENCHMARK
            benchmarkPoolEventsMs = runBenchmark(&Game::poolEvents, this, sfWindow);
            ImGui::SFML::Update(sfWindow, time->getDeltaClock().getElapsedTime());
            benchmarkUpdateMs = runBenchmark(&Game::onUpdate, this, time->getDelta());
            benchmarkInputMs = runBenchmark(&SceneManager::onInput, sceneManager, *inputManager);
            benchmarkRenderMs = runBenchmark(&Game::onRender, this, sfWindow);
            benchmarkSoundsMs = runBenchmark(&SoundManager::onUpdate, soundManager);
#else
            poolEvents(sfWindow);
            sceneManager->onInput(*inputManager);
            onUpdate(time->getDelta());
            onRender(sfWindow);
            soundManager->onUpdate();
#endif
        }
    }

    void Game::onRender(sf::RenderWindow& window)
    {          
        // Passing scene to PEM
        if (postEffectManager->isEnable())
        {
            auto target = postEffectManager->begin();
            sceneManager->onRender(*target);              
            postEffectManager->end();
        }      

        window.clear();
        // Scene drawaing 
        {
            if (!postEffectManager->isEnable())
                sceneManager->onRender(window);
            else 
                postEffectManager->draw(window);
        }

        // Debug collision grid 
        if (showColliderGrid)        
            collisionGrid->onRender(window);
                
        ImGui::SFML::Render(window);
        window.display();
    }
    
    void Game::onUpdate(float delta)
    {
        inputManager->update();
        sceneManager->onUpdate(delta);
        coroutineScheduler->update();
        
        collisionGrid->checkCollisions();

#ifdef USE_BENCHMARK        
        fpsDebugTextComp->setText("Fps:{}\nDelta:{:.4f}\nTime:{:.4f}\nRender:{:.2f}ms\nUpdate:{:.2f}ms\nPoolEvents:{:.2f}ms\nInput:{:.2f}ms\nSounds:{:.2f}ms\nSound Count:{}", 
            time->getFps(), delta, time->getTime(), benchmarkRenderMs, benchmarkUpdateMs, benchmarkPoolEventsMs, benchmarkInputMs, benchmarkSoundsMs, soundManager->size());
#else^
        fpsDebugTextComp->setText("Fps:{}\nDelta:{}", time->getFps(), delta);
#endif
        
        if (showDebugInfo)
        {
            inputDebugTextComp->setText(inputManager->toString());
            sceneDebugTextComp->setText(sceneManager->toString());
        }
    }
    
    void Game::poolEvents(sf::RenderWindow& window)
    {
        while (const auto event = window.pollEvent())
        {
            const auto pureEvent = *event;
            ImGui::SFML::ProcessEvent(window, pureEvent);
            
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