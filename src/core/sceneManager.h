#pragma once 
#include "common.h"
#include "gameObject.h"
#include <vector>
#include <ranges>

namespace dmsh::core
{
    // TODO: make it like container 
    struct Scene 
    {
        std::vector<std::weak_ptr<GameObject>> GameObjects;
    };

    class InputManager;
    class SceneManager : public Singleton<SceneManager>
    {
        public:
            SceneManager() { }
            ~SceneManager(); 
            
            void onResolutionChange(const sf::Vector2u& size);
            void onInput(InputManager& input);
            void onMouseClicked(sf::RenderWindow& window);
            void onRender(sf::RenderTarget& window);
            void onUpdate(float delta);
            void set(Scene&& scene);            
            void rebuildZOrdering();

            inline Scene& getScene() { return m_scene; }
        private:
            Scene m_scene;

        public:
            template<typename T>
            inline std::shared_ptr<T> createGameObject()
            {
                static_assert(std::is_same<GameObject, T>::value 
                    || std::is_base_of<GameObject, T>::value, "T must be GameObject or derived from GameObject");
                
                auto go = std::make_shared<T>();
                static std::uint32_t id = 0;
                go->initialStart();
                go->m_id = id;
                
                DMSH_DEBUG("create game object %i", id);
                id++;
                
                m_scene.GameObjects.shrink_to_fit();  
                m_scene.GameObjects.push_back(go);
                
                rebuildZOrdering();
                return go;
            }
            
            inline void deleteGameObject(std::shared_ptr<GameObject> go)
            {
                if (!go)
                    return;

                auto it = std::find_if(m_scene.GameObjects.begin(), m_scene.GameObjects.end(), [&](const std::weak_ptr<GameObject>& weak) {
                    if (!weak.expired())
                    {
                        auto locked = weak.lock();
                        if (locked) 
                        {
                            return locked == go; 
                        }
                    }
                
                    return false;  
                });

                if (it != m_scene.GameObjects.end())
                {
                    if (!it->expired())
                    {
                        it->lock()->onDestroy();
                    }
                    m_scene.GameObjects.erase(it);
                }

                // Also remove expired objects
                std::erase_if(m_scene.GameObjects, [&](const std::weak_ptr<GameObject>& weak){ 
                    return weak.expired();
                });

                m_scene.GameObjects.shrink_to_fit();  
                rebuildZOrdering();
            }

            inline std::string toString() const 
            {
                std::stringstream out;
                std::int32_t index = 0;
                out << "size:" << m_scene.GameObjects.size() << "\n";
                for (auto goWeak : m_scene.GameObjects)
                {
                    auto go = goWeak.lock();
                    if (!go)
                        continue;

                    out << index << ": z:" << go->getZDepth() << " id:" << go->m_id << "\n";
                    index++; 
                }
                
                return out.str();
            }
    };
}