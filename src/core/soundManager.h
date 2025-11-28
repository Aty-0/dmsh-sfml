#pragma once
#include "common.h"
#include <vector>
#include <memory>
#include <optional>

namespace dmsh::core
{
    struct SoundHeader 
    {
        std::shared_ptr<sf::Sound> sound;
        std::uint32_t id;
        bool removeOnStop;
    };

    class SoundManager : public Singleton<SoundManager>
    {
        public:
            SoundManager() = default;
            ~SoundManager();

            std::optional<SoundHeader> create(const sf::SoundBuffer& buffer, bool removeOnStop);
            void play(const SoundHeader& header, bool replayIfPlaying = false);
            void pause(const SoundHeader& header);
            void stop(const SoundHeader& header);
            void remove(const SoundHeader& header);

            void onUpdate();
            inline std::size_t size() const { return m_sounds.size(); }
        private:
            std::vector<std::pair<std::uint32_t, SoundHeader>> m_sounds;
            std::optional<SoundHeader> getSoundHeader(std::uint32_t id);
    };
}