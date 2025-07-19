#pragma once 
#include "common.h"
#include "zDepth.h"
#include "logger.h"

#include "component.h"
#include "drawable.h"
#include "transform.h"
#include "collider.h"

#include <memory>

namespace dmsh::core
{
    class GameObject : public zDepth, public std::enable_shared_from_this<GameObject>
    {
        public:
            GameObject() : m_tag("Default"), 
                m_transform(nullptr), m_drawable(nullptr)
            {
            }
            
            virtual ~GameObject()
            {
                m_drawable.reset();
                m_transform.reset();
            }

            virtual void onStart()
            {

            }
            
            virtual void onRender(sf::RenderWindow& window) 
            {
                for (auto component : m_components)
                {
                    if (component != nullptr)
                        component->onRender(window);
                }
            }
        
            virtual void onUpdate(float delta)
            {
                for (auto component : m_components)
                {
                    if (component != nullptr)
                        component->onUpdate(delta);
                }
            }

            virtual void onMouseClicked(const sf::Vector2f& pos)
            {
                for (auto component : m_components)
                {
                    if (component != nullptr)
                        component->onMouseClicked(pos);
                }
            }

            virtual void onMouseSelected(const sf::Vector2f& pos)
            {
                for (auto component : m_components)
                {
                    if (component != nullptr)
                        component->onMouseSelected(pos);
                }
            }

            virtual void onCollisionExit(const Collider& collider) 
            {
                for (auto component : m_components)
                {
                    if (component != nullptr)
                        component->onCollisionExit(collider);
                }
            }
            
            virtual void onCollisionStay(const Collider& collider)
            {
                for (auto component : m_components)
                {
                    if (component != nullptr)
                        component->onCollisionStay(collider);             
                }
            } 
            
            virtual void onCollisionEnter(const Collider& collider) 
            {
                for (auto component : m_components)
                {
                    if (component != nullptr)
                        component->onCollisionEnter(collider);
                }
            }
        
            inline Drawable& getDrawable() const { return *m_drawable; }
            inline Transform& getTransform() const { return *m_transform; }
            inline std::string getTag() const { return m_tag; }
            inline void setTag(const std::string& tag) { m_tag = tag; }
        private:
            friend class SceneManager;
            inline void initialStart()
            {
                m_drawable = createComponent<Drawable>();
                m_transform = createComponent<Transform>();
                // Set callbacks for drawable 
                m_transform->onPositionChanged = std::bind(&Drawable::updatePositionForTransformable, m_drawable.get(), std::placeholders::_1);
                m_transform->onRotationChanged = std::bind(&Drawable::updateRotationForTransformable, m_drawable.get(), std::placeholders::_1);
                m_transform->onScaleChanged = std::bind(&Drawable::updateScaleForTransformable, m_drawable.get(), std::placeholders::_1);
                // Invoke all callbacks to set initial values for drawable 
                m_transform->invokeCallbacks();            
                onStart();
            }
            
            std::string m_tag;
            std::int32_t m_id;
            // Base components
            std::shared_ptr<Drawable> m_drawable;
            std::shared_ptr<Transform> m_transform;

            std::vector<std::shared_ptr<Component>> m_components;
    
        public:
            template <typename T, typename... Args>
            inline std::shared_ptr<T> createComponent(Args&&... args) 
            {
                static_assert(std::is_base_of<Component, T>::value, "class must be based on Component");                
                auto component = std::make_shared<T>(std::forward<Args>(args)...);
                component->setOwner(shared_from_this());
                component->onStart();
                m_components.push_back(component);
                return component;
            }
            
            template <typename T, typename... Args>
            inline std::shared_ptr<T> addComponent(std::shared_ptr<T> component, Args&&... args) const
            {
                static_assert(std::is_base_of<Component, T>::value, "class must be based on Component");                
                component->setOwner(shared_from_this());
                component->onStart();
                m_components.push_back(component);
                return component;
            }
            
            template <typename T>
            inline std::shared_ptr<T> getComponent() const
            {
                static_assert(std::is_base_of<Component, T>::value, "class must be based on Component");                
                for (const auto& component : m_components)
                {
                    if (auto derived = std::dynamic_pointer_cast<T>(component))
                    {
                        return derived;
                    }
                }
                return nullptr;
            }
    };
}