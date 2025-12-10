#include "gameSandbox.h"
#include "player.h"
#include "nodeEditor.h"
#include "stageLoader.h"
#include "enemy.h"

#include "../core/window.h"
#include "../core/timer.h"
#include "../core/postEffects.h"

namespace dmsh::game
{
    void testFunc()
    {
        DMSH_DEBUG("test");    
    }
    
    void GameSandbox::onStart() 
    {
        const auto sceneManager = core::SceneManager::getInstance(); 
        const auto postEffectManager = core::PostEffectManager::getInstance(); 
        const auto resourceManager = core::ResourceManager::getInstance();
        const auto window = core::Window::getInstance();

        resourceManager->load<core::ResourceTypes::Sound>("sounds/damage00.wav", "damage");
        resourceManager->load<core::ResourceTypes::Texture>("textures/player.png", "player");
        resourceManager->load<core::ResourceTypes::Texture>("textures/enemy_1.png", "enemy_1");
        resourceManager->load<core::ResourceTypes::Texture>("textures/bullet.png", "bullet");
        resourceManager->load<core::ResourceTypes::Shader>("shaders/pt.frag", "pt", sf::Shader::Type::Fragment);
        resourceManager->load<core::ResourceTypes::Shader>("shaders/grain.frag", "grain", sf::Shader::Type::Fragment);
        resourceManager->load<core::ResourceTypes::Shader>("shaders/ca.frag", "ca", sf::Shader::Type::Fragment);
        resourceManager->load<core::ResourceTypes::Shader>("shaders/milk.frag", "milk", sf::Shader::Type::Fragment);

        // TODO: Remove asserts, if we are implement default resource getting 
        const auto grainShaderResource = resourceManager->get<core::ResourceTypes::Shader>("grain");
        DMSH_ASSERT(grainShaderResource, "Failed to load grain shader");
        const auto grainShader = grainShaderResource->getHandle();
        
        const auto caShaderResource = resourceManager->get<core::ResourceTypes::Shader>("ca");
        DMSH_ASSERT(caShaderResource, "Failed to load chromatic aberration shader");
        const auto chromaticAbShader = caShaderResource->getHandle();

        const auto milkShaderResource = resourceManager->get<core::ResourceTypes::Shader>("milk");
        DMSH_ASSERT(milkShaderResource, "Failed to load milk shader");
        const auto milkShader = milkShaderResource->getHandle();

        // TODO: Need to avoid getting window size
        const auto windowSize = window->getSize();    
        postEffectManager->create(windowSize, grainShader);
        //postEffectManager->create(windowSize, milkShader);        
        postEffectManager->create(windowSize, chromaticAbShader);        

        auto playerGo = sceneManager->createGameObject<core::GameObject>();
        playerGo->createComponent<game::Player>();


        for (std::int32_t i = 0; i < 10; ++i)
        {
            for (std::int32_t j = 0; j < 10; ++j)
            {
                auto npc = sceneManager->createGameObject<core::GameObject>();
                auto transform = npc->getTransform();
                npc->createComponent<game::Enemy>();
                transform->setPosition({100.0f + (j * 70), 100.0f + (i * 60)});
            }
        } 
        
        auto testTimer = playerGo->createComponent<core::Timer>();
        testTimer->onTimerUpdate.connect(testFunc);
        testTimer->setTime(10.0f);
        testTimer->run();
    }

    void GameSandbox::onDestroy() { } 
    void GameSandbox::onUpdate(float delta) { }
    void GameSandbox::onRender(sf::RenderTarget& target) { }    
    void GameSandbox::onInput() { }    
    void GameSandbox::onGui() { }

    void EditorSandbox::onStart() 
    {
        const auto resourceManager = core::ResourceManager::getInstance();
        resourceManager->load<core::ResourceTypes::Texture>("textures/node.png", "node");
        

        const auto sceneManager = core::SceneManager::getInstance(); 
        auto nodeEditor = sceneManager->createGameObject<core::GameObject>();
        auto nodeEditorComp = nodeEditor->createComponent<game::NodeEditor>();
        
        const auto stageLoader = game::StageLoader::getInstance();
        stageLoader->setNodeEditorComponentPtr(nodeEditorComp);
    }

    void EditorSandbox::onDestroy() { } 
    void EditorSandbox::onUpdate(float delta) { }
    void EditorSandbox::onRender(sf::RenderTarget& target) { }    
    void EditorSandbox::onInput() { }    
    void EditorSandbox::onGui() { }
}