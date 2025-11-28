#include "soundSource.h"
#include "logger.h"

namespace dmsh::core
{
    static auto resourceManager = core::ResourceManager::getInstance();
    static auto soundManager = core::SoundManager::getInstance();
    
    void SoundSource::onStart()
    {
    
    }
    
    void SoundSource::onDestroy()  
    {
        soundManager->remove(m_sound.value());
    }

    void SoundSource::stop(bool removeOnStop)
    {
        if (m_sound.has_value())    
        {
            soundManager->stop(m_sound.value());
            if (removeOnStop)
                soundManager->remove(m_sound.value());
        }
    }

    void SoundSource::pause()
    {
        if (m_sound.has_value())    
            soundManager->pause(m_sound.value());
    }

    void SoundSource::play(bool replayIfPlaying)
    {
        if (m_sound.has_value())    
            soundManager->play(m_sound.value(), replayIfPlaying);
    }

    void SoundSource::setSound(std::string_view name)
    {
        if (m_sound.has_value())
        {
            m_sound.value().sound->stop();
        }

        auto soundResource = resourceManager->get<core::ResourceTypes::Sound>(name);
        if (!soundResource)
            return;

        auto soundBuffer = soundResource->getHandle();
        DMSH_ASSERT(soundBuffer, "Sound buffer handle is invalid");
            
        m_sound = soundManager->create(*soundBuffer, false);
    }
}