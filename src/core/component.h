#pragma once
#include "common.h"
#include "logger.h"

namespace dmsh::core
{
    class GameObject;
    class InputManager;
    class Collider;
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
                DMSH_ASSERT(m_owner, "Invalid owner pointer"); 
                return m_owner; 
            }

            virtual void onStart() {} 
            virtual void onDestroy() {} 
            virtual void onInput(InputManager& input) {}
            virtual void onRender(sf::RenderWindow& window) {} 
            virtual void onUpdate(float delta) {}
            virtual void onMouseUnselected(const sf::Vector2f& pos) {}
            virtual void onMouseSelected(const sf::Vector2f& pos) {}
            virtual void onMouseClicked(const sf::Vector2f& pos) {}
            virtual void onCollisionExit(const Collider& collider) {}
            virtual void onCollisionStay(const Collider& collider) {}
            virtual void onCollisionEnter(const Collider& collider) {}               
        
        protected:
            template <typename T>
            inline std::shared_ptr<T> getSelf() const 
            { 
                DMSH_ASSERT(m_self, "Invalid self pointer"); 
                return std::dynamic_pointer_cast<T>(m_self); 
            }

        private:
            std::shared_ptr<GameObject> m_owner;
            std::shared_ptr<Component> m_self;
            
            friend class GameObject;
            inline void setSelf(std::shared_ptr<Component> self)
            {
                m_self = self;
            } 

            inline void setOwner(std::shared_ptr<GameObject>&& go) 
            {
                DMSH_ASSERT(go, "Owner is invalid"); 
                m_owner = std::move(go); 
            } 
    };
}