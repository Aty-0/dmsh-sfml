#include "animatedSpriteTexture.h"
#include "time.h"

namespace dmsh::core
{
    static const auto coroutineScheduler = core::coroutines::CoroutineScheduler::getInstance();
  
    void AnimatedSpriteTexture::setRectList(const std::initializer_list<sf::IntRect>& textureRectList)
    {
        DMSH_ASSERT(textureRectList.size() != 0, "Empty rect list");
        stop();
        m_rectList = std::move(textureRectList);
        m_sprite.setTextureRect(m_rectList[0]);
        run();
    }

    void AnimatedSpriteTexture::run()
    {
        m_animationCoroutine = coroutineScheduler->run(std::move(std::bind(&AnimatedSpriteTexture::moveRect, this)));
    }

    void AnimatedSpriteTexture::stop()
    {
        coroutineScheduler->stop(m_animationCoroutine);
    }

    core::coroutines::Coroutine AnimatedSpriteTexture::moveRect()
    {
        static const auto time = core::Time::getInstance();

        float sec = 0.0f; 
        std::int32_t rectIndex = 0;       
        while (true)
        {
            sec += m_speed * time->getDelta();
            if (sec > 1.0f)
            {
                if (rectIndex >= m_rectList.size())
                    rectIndex = 0;

                m_sprite.setTextureRect(m_rectList[rectIndex]);
                sec = 0.0f;
                rectIndex++;
            }

            co_await core::coroutines::Continue();
        }
    }
}