#include "coroutine.h"

namespace dmsh::core::coroutines
{
    void CoroutineScheduler::update()
    {
        auto it = m_coroutines.begin();
        auto clock = std::chrono::high_resolution_clock::now();

        while(it != m_coroutines.end())
        {            
            if(!it->second || it->second->is_done())       
            {
                //DMSH_DEBUG("is done, remove coroutine...");
                it = m_coroutines.erase(it);                            
            }     
            else if (it->second->should_resume(clock))
            {
                it->second->resume();
                ++it;
            }            
            else 
            {
                ++it;
            }            
        }
    }

    void CoroutineScheduler::stop(std::shared_ptr<Coroutine> coroutinePtr)
    {
        if (coroutinePtr == nullptr)
        {            
            return;
        }
        
        auto find = std::find_if(m_coroutines.begin(), m_coroutines.end(), [coroutinePtr](auto it) {
            return it.second == coroutinePtr;
        });

        if (find == m_coroutines.end())
        {
            //DMSH_ERROR("can't find coroutine in map");
            return;
        }

        m_coroutines.erase(find);
        coroutinePtr.reset();
    }

    std::shared_ptr<Coroutine> CoroutineScheduler::run(CoroutineFunction&& coroutineFunction)
    {
        const auto hashCode = coroutineFunction.target_type().hash_code();

        // Don't start a existing coroutine if it's not done 
        if (m_coroutines.find(hashCode) != m_coroutines.end())
        {
            const auto findedCoroutine = m_coroutines[hashCode];
            if (!findedCoroutine)
            {
                DMSH_ERROR("coroutine is finded in list, but it's empty, hm, continue %s", coroutineFunction.target_type().name());
            }
            else if (!findedCoroutine->is_done())
            {
                //DMSH_DEBUG("too bad, it's not done %s", coroutineFunction.target_type().name());
                return findedCoroutine; 
            }
        }

        const auto coroutineShared = std::make_shared<Coroutine>(std::move(coroutineFunction()));
        m_coroutines.insert({ hashCode, coroutineShared });
        return coroutineShared;
    } 
}