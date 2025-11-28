#include "soundManager.h"
#include "logger.h"

namespace dmsh::core
{
    SoundManager::~SoundManager()
    {
        m_sounds.clear();
        m_sounds.shrink_to_fit();
    }
    
    void SoundManager::remove(const SoundHeader& header)
    {
        //DMSH_DEBUG("Remove sound %i", header.id);
        std::erase_if(m_sounds, [&header](const auto& el) { 
            return el.first == header.id; 
        });        
    }
    
    void SoundManager::onUpdate()
    {
        for (std::int32_t i = 0; i < m_sounds.size(); ++i)
        {
            auto header = m_sounds[i].second;
            // Empty sound will be removed
            if (!header.sound)
            { 
                m_sounds.erase(m_sounds.begin() + i);
                continue;
            }
            auto status = header.sound->getStatus();
            //DMSH_DEBUG("sound id: %i status:%i", header.id, status);
            // TODO: Callbacks
            switch (status)
            {
                case sf::Sound::Status::Stopped:
                    if (header.removeOnStop)                    
                        remove(header);                    
                    break;
                case sf::Sound::Status::Playing:
                    break;
                case sf::Sound::Status::Paused:
                    break;
            }
        }
    }

    constexpr std::uint16_t MAX_SIZE = 256;
    std::optional<SoundHeader> SoundManager::create(const sf::SoundBuffer& buffer, bool removeOnStop)
    {
        static std::uint32_t id = 0;
        //DMSH_DEBUG("Create sound %i", id);

        if (m_sounds.size() > MAX_SIZE)
        {
            DMSH_DEBUG("max sound size reached!");
            return std::nullopt;
        }
        
        auto sound = std::make_shared<sf::Sound>(buffer);
        sound->setLooping(false);

        auto header = SoundHeader {
            std::move(sound),
            id,
            removeOnStop,
        };

        m_sounds.push_back({id, header});
        id++;
        return header;
    }

    void SoundManager::play(const SoundHeader& header, bool replayIfPlaying)
    {
        auto result = getSoundHeader(header.id);
        if (result.has_value())
        {
            if (!replayIfPlaying && header.sound->getStatus() == sf::Sound::Status::Playing)
                return;

            result.value().sound->play();
        }
    }

    void SoundManager::stop(const SoundHeader& header)
    {
        auto result = getSoundHeader(header.id);
        if (result.has_value())
            result.value().sound->stop();        
    }

    void SoundManager::pause(const SoundHeader& header)
    {
        auto result = getSoundHeader(header.id);
        if (result.has_value())
            result.value().sound->pause();        
    }
    
    std::optional<SoundHeader> SoundManager::getSoundHeader(std::uint32_t id) 
    {
        auto it = std::find_if(m_sounds.begin(), m_sounds.end(), [id](const auto& el) { return el.first == id; });
        if (it == m_sounds.end() || !it->second.sound)     
            return std::nullopt;

        return it->second;
    }    
}