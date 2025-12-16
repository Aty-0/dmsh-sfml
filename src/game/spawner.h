#pragma once 
#include "../core/sceneManager.h"
#include "../core/component.h"
#include "enemyNode.h"

namespace dmsh::game
{    
    class Spawner : public core::Component
    {
        public:
            

        private:
            std::vector<core::GameObject> m_gameObjects;
    };
}