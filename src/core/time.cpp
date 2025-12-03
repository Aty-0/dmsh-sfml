#include "time.h"

namespace dmsh::core
{
    void Time::update()
    {
        static std::int32_t frames = 0;
        
        frames++;
        if (m_fpsClock.getElapsedTime() >= sf::seconds(1))
        {
            m_lastFrameCount = frames;
            frames = 0;
            m_fpsClock.restart();
        }
        
        m_delta = m_deltaClock.getElapsedTime().asSeconds();
        m_time += m_delta;
        m_deltaClock.restart();
    }
}