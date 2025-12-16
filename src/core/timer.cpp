#include "timer.h"

namespace dmsh::core
{
    static const auto coroutineScheduler = core::coroutines::CoroutineScheduler::getInstance();

    void Timer::restart()
    {
        stop();
        reset();
        run();
    }

    void Timer::reset()
    {
        m_isEnded = false;
        m_currentTime = m_time;
    }

    void Timer::run()
    {
        m_tickCoroutine = coroutineScheduler->run(std::move(std::bind(&Timer::onTick, this)));
    }

    void Timer::stop()
    {
        if (m_tickCoroutine)
        {
            coroutineScheduler->stop(m_tickCoroutine);
        }
    }

    void Timer::onStart()  
    {
        m_currentTime = m_time;
    }

    core::coroutines::Coroutine Timer::onTick()
    {
        onTimerStart();
        
        while (m_currentTime >= 0)
        {
            //DMSH_DEBUG("%f, %f", m_time, m_currentTime);
            m_currentTime -= m_tick;
            onTimerUpdate();

            co_await core::coroutines::WaitForSeconds(m_tick);
        }

        m_isEnded = true;

        onTimerEnd();
    }
}