#pragma once 
#include "common.h"
#include "zDepth.h"
#include "logger.h"

#include "component.h"
#include "drawable.h"
#include "transform.h"
#include "collider.h"
#include "view.h"

#include <memory>

namespace dmsh::core
{
    class InputManager;
    class GameObject : public zDepth, public std::enable_shared_from_this<GameObject>
    {
        public:
            GameObject() :
                m_tag("Default"), 
                m_transform(nullptr), 
                m_drawable(nullptr),
                m_visible(true),
                m_id(-1) // By default it's a game viewspace 
            {
                setViewSpace(core::getViewSpaceGame());                
            }
            
            virtual ~GameObject()
            {
                onDestroy();
                
                m_drawable.reset();
                m_transform.reset();

                m_components.clear();
                m_components.shrink_to_fit();
            }

            virtual void onDestroy()
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onDestroy();
                }
            }


            virtual void onStart()
            {
                // Nothing here
            }

            virtual void onInput(InputManager& input)
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onInput(input);
                }
            }
            
            virtual void onRender(sf::RenderWindow& window) 
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;      

                    component->onRender(window);
                }
            }
        
            virtual void onUpdate(float delta)
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onUpdate(delta);
                    component->onGui();
                }
            }

            virtual void onMouseClicked(const sf::Vector2f& pos)
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onMouseClicked(pos);
                }
            }

            virtual void onMouseUnselected(const sf::Vector2f& pos)
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onMouseUnselected(pos);
                }
            }

            virtual void onMouseSelected(const sf::Vector2f& pos)
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onMouseSelected(pos);
                }
            }

            virtual void onCollisionExit(const Collider& collider) 
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onCollisionExit(collider);
                }
            }
            
            virtual void onCollisionStay(const Collider& collider)
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;

                    component->onCollisionStay(collider);             
                }
            } 
            
            virtual void onCollisionEnter(const Collider& collider) 
            {
                for (auto component : m_components)
                {
                    if (!component || !m_visible)
                        continue;
                    
                    component->onCollisionEnter(collider);
                }
            }
        
            inline std::shared_ptr<Drawable> getDrawable() const { return m_drawable; }
            inline std::shared_ptr<Transform> getTransform() const { return m_transform; }

            // TODO: Tags; Rework, use macro or classes
            inline std::string getTag() const { return m_tag; }
            inline void setTag(const std::string& tag) { m_tag = tag; }
            inline bool isVisible() const { return m_visible; }
            inline void setVisible(bool visible) { m_visible = visible; }
            
            // FIXME: UUID
            inline std::int32_t getId() const { return m_id; }

            inline void setViewSpace(const core::ViewSpace& view) { m_view = &view; }
            // Get viewspace which object is using  
            inline const core::ViewSpace& getViewSpace() const 
            { 
                DMSH_ASSERT(m_view, "ViewSpace is empty");
                return *m_view; 
            }

        private:
            friend class SceneManager;
            inline void initialStart()
            {
                m_drawable = createComponent<Drawable>();
                m_transform = createComponent<Transform>();

                // Set callbacks for drawable 
                m_transform->Signals.onPositionChanged.connect(&Drawable::updatePositionForTransformable, m_drawable.get());
                m_transform->Signals.onRotationChanged.connect(&Drawable::updateRotationForTransformable, m_drawable.get());
                m_transform->Signals.onScaleChanged.connect(&Drawable::updateScaleForTransformable, m_drawable.get());

                // Invoke all callbacks to set initial values for drawable 
                m_transform->invokeCallbacks();                            

                onStart();
            }

   
            bool m_visible;
            std::string m_tag;
            std::int32_t m_id;
            // Base components
            std::shared_ptr<Drawable> m_drawable;
            std::shared_ptr<Transform> m_transform;

            std::vector<std::shared_ptr<Component>> m_components;
            
            const core::ViewSpace* m_view;

        public:
            template <typename T, typename... Args>
            inline std::shared_ptr<T> createComponent(Args&&... args) 
            {
                static_assert(std::is_base_of<Component, T>::value, "class must be based on Component");                
                auto component = std::make_shared<T>(std::forward<Args>(args)...);
                addComponent(component, std::forward<Args>(args)...);
                return component;
            }
            
            template <typename T, typename... Args>
            inline void addComponent(std::shared_ptr<T> component, Args&&... args)
            {
                static_assert(std::is_base_of<Component, T>::value, "class must be based on Component");                
                auto self = shared_from_this();
                DMSH_ASSERT(self, "self is invalid");
                component->setOwner(std::move(self));
                component->setSelf(component);
                component->onStart();
                m_components.push_back(std::move(component));
            }
            
            template <typename T>
            inline std::shared_ptr<T> getComponent() const
            {
                static_assert(std::is_base_of<Component, T>::value, "class must be based on Component");                
                for (auto component : m_components)
                {
                    if (!component) 
                    {
                        continue;
                    }

                    const auto derived = std::dynamic_pointer_cast<T>(component);
                    if (derived != nullptr)
                    {
                        return derived;
                    }
                }
                
                return nullptr;
            }
    };
}