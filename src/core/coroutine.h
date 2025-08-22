#pragma once
#include <coroutine>
#include <optional>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

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
        
            ~Coroutine() 
            {
                if (handle) 
                { 
                    handle.destroy(); 
                }
            }

            inline Coroutine& operator=(Coroutine&& other) noexcept
            {
                if (this != &other)
                {
                    if (handle) handle.destroy();
                    handle = other.handle;
                    other.handle = nullptr;
                }
            
                return *this;
            }

            inline bool is_done() const
            {
                return !handle || handle.done();
            }

            inline void resume()
            {
                if (handle)
                {
                    handle.resume();
                }
            }

            inline bool should_resume(std::chrono::high_resolution_clock::time_point now) const 
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
            std::shared_ptr<Coroutine> run(CoroutineFunction&& coroutineFunction);
            void stop(std::shared_ptr<Coroutine> coroutinePtr);

        private:
            std::unordered_map<std::size_t, std::shared_ptr<Coroutine>> m_coroutines;
    };


    struct Continue
    {    
        inline bool await_ready() const noexcept { return false; }
        inline void await_resume() const noexcept {}    
        inline void await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) const noexcept {} 
    };

    struct WaitForSeconds
    {    
        std::chrono::high_resolution_clock::duration duration;
        explicit WaitForSeconds(float seconds) 
            : duration(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
                std::chrono::duration<float>(seconds))) 
        {
            
        }
        inline bool await_ready() const noexcept { return false; }
        inline void await_resume() const noexcept {}    
        inline void await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) const noexcept 
        {
            handle.promise().resume_time = std::chrono::high_resolution_clock::now() + duration;
        }        
    };

}