#pragma once
#include "common.h"
#include "logger.h"

namespace dmsh::core
{
    class GameObject;
    struct Collider;
    class Component 
    {    
        public:    
            Component() { } 
            virtual ~Component()
            {
                m_owner = nullptr;
            }

            inline std::shared_ptr<GameObject> getOwner() const 
            {
                DMSH_ASSERT(m_owner, "No owner"); 
                return m_owner; 
            }

            virtual void onStart() {} 
            virtual void onDestroy() {} 
            virtual void onRender(sf::RenderWindow& window) {} 
            virtual void onUpdate(float delta) {}
            virtual void onMouseSelected(const sf::Vector2f& pos) {}
            virtual void onMouseClicked(const sf::Vector2f& pos) {}
            virtual void onCollisionExit(const Collider& collider) {}
            virtual void onCollisionStay(const Collider& collider) {}
            virtual void onCollisionEnter(const Collider& collider) {}               
        
        private:
            std::shared_ptr<GameObject> m_owner;
            
            friend class GameObject;
            inline void setOwner(std::shared_ptr<GameObject> go) 
            {
                DMSH_ASSERT(go, "go is nullptr"); 
                m_owner = go; 
            } 
    };
}