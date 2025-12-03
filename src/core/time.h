#pragma once
#include "common.h"

namespace dmsh::core
{
    class Time : public Singleton<Time>
    {
        public:
            void update();

            inline std::int32_t getFps() const { return m_lastFrameCount; }
            inline float getDelta() const { return m_delta; }
            inline float getTime() const { return m_time; }
            inline sf::Clock getFpsClock() const { return m_fpsClock; }    
            inline sf::Clock getDeltaClock() const { return m_deltaClock; }    

        private:
            sf::Clock m_fpsClock;    
            sf::Clock m_deltaClock;
            float m_delta;
            float m_time;
            std::int32_t m_lastFrameCount;    
    };
}