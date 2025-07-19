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
        std::vector<std::shared_ptr<GameObject>> GameObjects;
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
                    || std::is_base_of<GameObject, T>::value, "It's a not a game object or not a based of game object class");

                auto go = std::make_shared<T>();
                static std::uint32_t id = 0;
                go->initialStart();
                go->m_id = id;
                
                DMSH_DEBUG("create game object %i", id);
                id++;
                m_scene.GameObjects.push_back(go);
                return go;
            }
            
            inline void deleteGameObject(std::shared_ptr<GameObject> go)
            {
                if (go == nullptr)
                    return;

                std::erase(m_scene.GameObjects, go);
                go.reset();
            }

            inline std::string toString() const 
            {
                std::stringstream out;
                std::int32_t index = 0;
                for (auto go : m_scene.GameObjects)
                {
                    out << index << ": z:" << go->getZDepth() << " id:" << go->m_id << "\n";
                    index++; 
                }
                
                out << "\n" << "size:" << m_scene.GameObjects.size();
                return out.str();
            }
    };
}