#pragma once
#include "common.h"

namespace dmsh::core
{
    class Time : public Singleton<Time>
    {
        public:
            inline std::int32_t getFps() 
            {
                static std::int32_t frames = 0;
                static std::int32_t lastFrameCount = 0;
                
                frames++;
                if (m_fpsClock.getElapsedTime() >= sf::seconds(1))
                {
                    lastFrameCount = frames;
                    frames = 0;
                    m_fpsClock.restart();
                    return lastFrameCount;
                }
                
                return lastFrameCount;
            }

            inline float getDelta()  
            { 
                static float delta = 0.0f;
                delta = m_deltaClock.getElapsedTime().asSeconds();
                m_deltaClock.restart();
                return delta;
            } 
        
        private:
            sf::Clock m_fpsClock;    
            sf::Clock m_deltaClock;    
    };
}