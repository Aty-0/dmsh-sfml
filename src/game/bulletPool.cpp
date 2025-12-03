#include "bulletPool.h"
#include "../core/sceneManager.h"

namespace dmsh::game
{
    BulletPool::BulletPool(std::int32_t count)
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

    void BulletPool::pool(const sf::Vector2f& pos)
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

    void BulletPool::back(std::shared_ptr<core::GameObject> go)
    {
        go->setVisible(false);
    }
   
}