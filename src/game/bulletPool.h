#pragma once
#include "bullet.h"
#include <vector>

namespace dmsh::game
{
    class BulletPool
    {
        public:
            explicit BulletPool(std::int32_t count);
            void pool(const sf::Vector2f& pos);
            void back(std::shared_ptr<core::GameObject> go);
      
        private:
            std::vector<std::shared_ptr<core::GameObject>> m_pool;
    };
}