#pragma once
#include <coroutine>
#include <optional>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_set>

#include "logger.h"
#include "singleton.h"

namespace dmsh::core::coroutines
{
    class Coroutine 
    {
        private: 
            struct Promise 
            {
                std::chrono::high_resolution_clock::time_point resume_time;

                inline Coroutine get_return_object()
                {
                    return Coroutine(std::coroutine_handle<Promise>::from_promise(*this));
                }
                std::suspend_always initial_suspend() noexcept { return {}; }
                std::suspend_always final_suspend() noexcept { return {}; }
                void unhandled_exception() 
                {
                    try 
                    {
                        std::rethrow_exception(std::move(std::current_exception()));
                    }
                    catch (std::exception exp)
                    {
                        DMSH_FATAL("Coroutine exception: %s", exp.what());
                    }
                }
                void return_void() { }

            };

        public:
            using promise_type = Promise;
            
            explicit Coroutine(std::coroutine_handle<Promise> h)
                : handle (h)
            {

            }
            
            Coroutine(Coroutine&& other) noexcept : handle(other.handle)
            {
                other.handle = nullptr;
            }

            Coroutine& operator=(Coroutine&& other) noexcept
            {
                if (this != &other)
                {
                    if (handle) handle.destroy();
                    handle = other.handle;
                    other.handle = nullptr;
                }
            
                return *this;
            }
        
            ~Coroutine() 
            {
                if (handle) 
                { 
                    handle.destroy(); 
                }
            }

            bool is_done() const
            {
                return !handle || handle.done();
            }

            void resume()
            {
                if (handle)
                {
                    handle.resume();
                }
            }

            bool should_resume(std::chrono::high_resolution_clock::time_point now) const 
            {
                return !is_done() && now >= handle.promise().resume_time;
            }

        private:
            std::coroutine_handle<Promise> handle;
    };

    class CoroutineScheduler : public Singleton<CoroutineScheduler>
    {
        public:
            using CoroutineFunction = std::function<Coroutine()>;

            void update();
            void run(CoroutineFunction coroutine);

        private:
            std::unordered_set<std::shared_ptr<Coroutine>> m_coroutines;
    };

    inline void CoroutineScheduler::update()
    {
        auto it = m_coroutines.begin();
        auto clock = std::chrono::high_resolution_clock::now();

        while(it != m_coroutines.end())
        {
            if((*it)->is_done())       
            {
                //DMSH_DEBUG("is done, remove coroutine...");
                it = m_coroutines.erase(it);                            
            }     
            else if ((*it)->should_resume(clock))
            {
                (*it)->resume();
                ++it;
            }            
            else 
            {
                ++it;
            }            
        }
    }

    inline void CoroutineScheduler::run(CoroutineFunction coroutine)
    {
        m_coroutines.insert(std::make_shared<Coroutine>(coroutine()));
    } 

    struct Continue
    {    
        bool await_ready() const noexcept { return false; }
        void await_resume() const noexcept {}    
        void await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) const noexcept {} 
    };

    struct WaitForSeconds
    {    
        std::chrono::high_resolution_clock::duration duration;
        explicit WaitForSeconds(float seconds) 
            : duration(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::duration<float>(seconds))) 
        {
            
        }
        bool await_ready() const noexcept { return false; }
        void await_resume() const noexcept {}    
        void await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) const noexcept 
        {
            handle.promise().resume_time = std::chrono::high_resolution_clock::now() + duration;
        }        
    };

}