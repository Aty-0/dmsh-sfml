#include "sceneManager.h"
#include "logger.h"
#include "window.h"
#include "spatialGrid.h"

namespace dmsh::core
{    
    SceneManager::~SceneManager()
    {
        m_scene.GameObjects.clear();
        m_scene.GameObjects.shrink_to_fit();
    }
    
    void SceneManager::onInput(InputManager& input)
    {
        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i) 
        {
            auto weakGo = m_scene.GameObjects[i];
            if (weakGo.expired())
                continue;

            weakGo.lock()->onInput(input);
        }    
    }

    void SceneManager::onMouseClicked(sf::RenderWindow& window)
    {  
        static const auto gameWindow = core::Window::getInstance();
        const auto mousePos = gameWindow->getMousePosition();
        const auto worldCoords = window.mapPixelToCoords(mousePos);
        
        static std::shared_ptr<Collider> collidedInPrevFrame;
        std::shared_ptr<Collider> collided;

        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i) 
        {               
            auto& goWeak = m_scene.GameObjects[i];
            if (goWeak.expired())
                continue;

            const auto go = goWeak.lock();                
            go->onMouseClicked(worldCoords);
                
            auto collider = go->getComponent<Collider>();
            if (!collider)
                continue;
            
            if (collider->contains(worldCoords))
            {
                go->onMouseSelected(worldCoords);

                if (collidedInPrevFrame != collider)
                    collided = collider;
            }

            if (collided != collider) 
            {
                go->onMouseUnselected(worldCoords);
            }
        }

        collidedInPrevFrame = collided;
    }

    void SceneManager::set(Scene&& scene)
    {
        m_scene.GameObjects.clear();
        m_scene = std::move(scene);
    }

    void SceneManager::onRender(sf::RenderWindow& window)
    {
        //const core::ViewSpace* prevViewspace = nullptr;
        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i)         
        {            
            auto goWeak = m_scene.GameObjects[i];
            if (goWeak.expired())
                continue;
            auto goLock = goWeak.lock();

            auto current = &goLock->getViewSpace();
            // Do not change view if we are have the same viewspace 
            //if (prevViewspace 
            //    && prevViewspace != current)
            {
                window.setView(current->getView());
            }

            goLock->onRender(window);
            //prevViewspace = current;
        }
    }

    void SceneManager::onUpdate(float delta)
    {
        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i)         
        {
            auto goWeak = m_scene.GameObjects[i];
            if (goWeak.expired())
                continue;

            auto go = goWeak.lock();
            go->onUpdate(delta);                   
        }
    }

    void SceneManager::rebuildZOrdering()
    {
        //DMSH_DEBUG("rebuilding Z ordering...");        
        std::sort(m_scene.GameObjects.begin(), m_scene.GameObjects.end(), [](const auto& first, const auto& second) { 
            if (first.expired() || second.expired())
                return false;

            return first.lock()->getZDepth() < second.lock()->getZDepth();
        });
    }
}