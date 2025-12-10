#pragma once 
#include "common.h"
#include "coroutine.h"
#include "component.h"

namespace dmsh::core
{
    class Timer : public Component
    {
        public:
            Timer() : m_time(0.0f), m_currentTime(1.0f), m_tick(1.0f) { }
            
            virtual void onStart() override;
            
            inline float getTime() const { return m_time; }
            inline float getTick() const { return m_tick; }
            inline bool isEnded() const { return m_isEnded; }
            inline bool isStarted() const { return m_tickCoroutine != nullptr; }
            
            inline void setTime(float time) 
            { 
                m_time = time; 
                // FIXME: 
                m_currentTime = m_time; 
            }
            inline void setTick(float tick) { m_tick = tick; }

            void restart();
            void run();
            void reset();
            void stop();

        private:
            bool m_isEnded;
            float m_time;
            float m_currentTime;
            float m_tick;
            std::shared_ptr<core::coroutines::Coroutine> m_tickCoroutine;

            core::coroutines::Coroutine onTick();
        public: 
            sigslot::signal<> onTimerStart;
            sigslot::signal<> onTimerUpdate;
            sigslot::signal<> onTimerEnd;
    };
}