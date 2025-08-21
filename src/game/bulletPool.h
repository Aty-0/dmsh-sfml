#pragma once
#include "bullet.h"
#include <vector>

namespace dmsh::game
{
    class BulletPool
    {
        public:
            explicit BulletPool(std::int32_t count)
            {
                for (std::int32_t i = 0; i < count; ++i)
                {
                    static const auto sceneManager = core::SceneManager::getInstance();
                    auto go = sceneManager->createGameObject<core::GameObject>();
                    go->setVisible(false);
                    go->createComponent<Bullet>();
                    m_pool.push_back(go);                    
                }
            }

            void pool(const sf::Vector2f& pos)
            {
                static std::int32_t index = 0;
                auto go = m_pool[index]; 
                if (go == nullptr)
                {
                    // TODO: create
                    return;
                }

                go->getTransform()->setPosition(pos);
                go->setVisible(true);
                index++;
                if (index >= m_pool.size())
                {
                    index = 0;
                }
            }

            void back(std::shared_ptr<core::GameObject> go)
            {
                go->setVisible(false);
            }

        private:
            std::vector<std::shared_ptr<core::GameObject>> m_pool;
    };
}