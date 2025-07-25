#include "sceneManager.h"
#include "logger.h"
#include "window.h"
#include <unordered_set>

namespace dmsh::core
{    
    SceneManager::~SceneManager()
    {
        //collisionDetectedPrevFrame.clear();
        
        m_scene.GameObjects.clear();
        m_scene.GameObjects.shrink_to_fit();
    }
    
    void SceneManager::onInput(InputManager& input)
    {
        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i) 
        {
            auto& weak = m_scene.GameObjects[i];
            const auto go = weak.lock();
            if (!go)
                continue;

            go->onInput(input);
        }    
    }

    void SceneManager::onMouseClicked(sf::RenderWindow& window)
    {  
        static const auto gameWindow = core::Window::getInstance();
        const auto mousePos = gameWindow->getMousePosition();
        const auto worldCoords = window.mapPixelToCoords(mousePos);
        
        static Collider* collidedInPrevFrame = nullptr;;
        Collider* collided = nullptr;
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
            {
                go->onMouseSelected(worldCoords);

                if (collidedInPrevFrame != collider.get())
                    collided = collider.get();
            }

            if (collided != collider.get()) 
            {
                go->onMouseUnselected(worldCoords);
            }
        }

        collidedInPrevFrame = collided;
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
            
            collider->m_collisionTracker.Collided.clear();            

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
                    if (!collider->m_collisionTracker.isObjectCollidedPrevFrame(otherCollider.get()))
                    {
                        DMSH_DEBUG("it's a on collision enter callback");
                        go->onCollisionEnter(*otherCollider);
                    }

                    // Invoke a stay collision callback
                    go->onCollisionStay(*otherCollider);
                    collider->m_collisionTracker.Collided.insert(otherCollider.get());
                }                                                            
            }
        }
        
        for (std::size_t i = 0; i < m_scene.GameObjects.size(); ++i)         
        {          
            auto& weak = m_scene.GameObjects[i];
            auto go = weak.lock();
            if (!go)
                continue;

            auto collider = go->getComponent<Collider>();
            if (!collider)
                continue;

            for (auto other : collider->m_collisionTracker.CollidedInPrevFrame) 
            {
                if (!collider->m_collisionTracker.isObjectCollided(other)) 
                {
                    // TODO: other->getCollider() it's a wrong I think
                    DMSH_DEBUG("it's a on collision exit callback");
                    other->onCollisionExit(*collider);
                }
            }
    
            collider->m_collisionTracker.CollidedInPrevFrame = collider->m_collisionTracker.Collided;
        }
    }

    void SceneManager::rebuildZOrdering()
    {
        //DMSH_DEBUG("rebuilding Z ordering...");        
        std::sort(m_scene.GameObjects.begin(), m_scene.GameObjects.end(), [](const auto& first, const auto& second) { 
            return first.lock()->getZDepth() < second.lock()->getZDepth();
        });
    }
}