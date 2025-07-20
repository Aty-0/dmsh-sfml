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

    class SceneManager : public Singleton<SceneManager>
    {
        public:
            SceneManager() { }
            ~SceneManager(); 
            void set(const Scene& scene);
            void onMouseClicked(sf::RenderWindow& window);
            void onRender(sf::RenderWindow& window);
            void onUpdate(float delta);
            void rebuildZOrdering();
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
                return go;
            }
            
            inline void deleteGameObject(std::shared_ptr<GameObject> go)
            {
                if (go == nullptr)
                    return;

                std::erase_if(m_scene.GameObjects, [&](const std::weak_ptr<GameObject>& weak) {
                        auto locked = weak.lock();
                        if (locked) 
                        {
                            return locked == go; 
                        }
                        // Remove expired ptr
                        return true;  
                });
            }

            inline std::string toString() const 
            {
                std::stringstream out;
                std::int32_t index = 0;
                for (auto goWeak : m_scene.GameObjects)
                {
                    auto go = goWeak.lock();
                    if (!go)
                        continue;

                    out << index << ": z:" << go->getZDepth() << " id:" << go->m_id << "\n";
                    index++; 
                }
                
                out << "\n" << "size:" << m_scene.GameObjects.size();
                return out.str();
            }
    };
}