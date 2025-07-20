#include "sceneManager.h"
#include "logger.h"
#include "window.h"
#include <unordered_set>

namespace dmsh::core
{
    static std::unordered_set<GameObject*> collisionDetectedPrevFrame;
    
    SceneManager::~SceneManager()
    {
        collisionDetectedPrevFrame.clear();
        
        m_scene.GameObjects.clear();
        m_scene.GameObjects.shrink_to_fit();
    }
    
    void SceneManager::onMouseClicked(sf::RenderWindow& window)
    {  
        static const auto gameWindow = core::Window::getInstance();
        const auto mousePos = gameWindow->getMousePosition();
        const auto worldCoords = window.mapPixelToCoords(mousePos);
        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i) 
        {               
            auto& weak = m_scene.GameObjects[i];
            const auto go = weak.lock();
            if (!go)
                continue;
                
            go->onMouseClicked(worldCoords);
                
            const auto collider = go->getComponent<Collider>();
            if (!collider)
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
        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i)         
        {            
            auto& weak = m_scene.GameObjects[i];
            if (auto go = weak.lock())
                go->onRender(window);
        }
    }
    
    void SceneManager::onUpdate(float delta)
    {
        std::unordered_set<GameObject*> collisionDetected;

        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i)         
        {
            auto& weak = m_scene.GameObjects[i];
            auto go = weak.lock();
            if (!go)
                continue;

            go->onUpdate(delta);
            
            auto collider = go->getComponent<Collider>();
            if (!collider)
                continue;

            for (std::size_t j = 0; j < m_scene.GameObjects.size(); ++j)         
            {
                auto& otherWeak = m_scene.GameObjects[j];
                auto other = otherWeak.lock();
                if (!other || other == go)
                    continue;
                                     
                auto otherCollider = other->getComponent<Collider>();
                if (!otherCollider)
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
                if (!collider)
                    other->onCollisionExit(*collider);
            }
        }

        collisionDetectedPrevFrame = collisionDetected;
    }

    void SceneManager::rebuildZOrdering()
    {
        DMSH_DEBUG("rebuilding Z ordering...");
        
        std::sort(m_scene.GameObjects.begin(), m_scene.GameObjects.end(), [](const auto& first, const auto& second) { 
            return first.lock()->getZDepth() < second.lock()->getZDepth();
        });
    }
}