#include "sceneManager.h"
#include "logger.h"
#include "window.h"
#include <unordered_set>

namespace dmsh::core
{
    static std::unordered_set<GameObject*> collisionDetectedPrevFrame;
    
    SceneManager::~SceneManager()
    {
        m_scene.GameObjects.clear();
        
        for (auto* go : collisionDetectedPrevFrame)
            go = nullptr;
        //collisionDetectedPrevFrame.clear();
    }
    
    void SceneManager::onMouseClicked(sf::RenderWindow& window)
    {
        static const auto gameWindow = core::Window::getInstance();
        const auto mousePos = gameWindow->getMousePosition();
        const auto worldCoords = window.mapPixelToCoords(mousePos);
        for (auto go : m_scene.GameObjects)
        {
            if (go == nullptr)
                continue;
                
            go->onMouseClicked(worldCoords);
                
            auto collider = go->getComponent<Collider>();
            if (collider == nullptr)
                continue;

            if (collider->contains(worldCoords))
                go->onMouseSelected(worldCoords);            
        }
    }

    void SceneManager::set(const Scene& scene)
    {
        m_scene.GameObjects.clear();
        m_scene = scene;
    }

    void SceneManager::onRender(sf::RenderWindow& window)
    {
        for (auto go : m_scene.GameObjects)
        {
            if (go != nullptr)
                go->onRender(window);
        }
    }
    
    void SceneManager::onUpdate(float delta)
    {
        std::unordered_set<GameObject*> collisionDetected;

        for (auto go : m_scene.GameObjects)
        {
            if (go == nullptr)
                continue;

            go->onUpdate(delta);
            
            auto collider = go->getComponent<Collider>();
            if (collider == nullptr)
                continue;

            for (auto other : m_scene.GameObjects)
            {
                if (other == nullptr || other == go)
                    continue;
                                     
                auto otherCollider = other->getComponent<Collider>();
                if (otherCollider == nullptr)
                    continue;

                if (collider->intersect(*otherCollider))
                {
                    // Try to find a game object pointer on a prev frame, if we are not found a go, 
                    // then we are invoke a start callback
                    if (collisionDetectedPrevFrame.count(go.get()) == 0)
                    {
                        DMSH_DEBUG("it's a on collision enter callback");
                        go->onCollisionEnter(*otherCollider);
                    }

                    // Invoke a stay collision callback
                    go->onCollisionStay(*otherCollider);
                    collisionDetected.insert(go.get());
                }                                                            
            }
        }
        

        for (auto other : collisionDetectedPrevFrame) 
        {
            if (collisionDetected.count(other) == 0) 
            {
                // TODO: other->getCollider() it's a wrong I think
                DMSH_DEBUG("it's a on collision exit callback");
                auto collider = other->getComponent<Collider>();
                if (collider != nullptr)
                    other->onCollisionExit(*collider);
            }
        }

        collisionDetectedPrevFrame = collisionDetected;
    }

    void SceneManager::rebuildZOrdering()
    {
        DMSH_DEBUG("rebuilding Z ordering...");
        
        std::sort(m_scene.GameObjects.begin(), m_scene.GameObjects.end(), [](const auto& first, const auto& second) { 
            return first->getZDepth() < second->getZDepth();
        });
    }
}