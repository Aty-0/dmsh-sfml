#pragma once
#include "component.h"
#include "resourceManager.h"
#include "soundManager.h"

namespace dmsh::core
{
    class SoundSource : public Component
    {
        public:
            virtual void onStart() override;
            virtual void onDestroy() override;

            void pause();
            void stop(bool removeOnStop = false);
            void play(bool replayIfPlaying = false);
            void setSound(std::string_view name);

            inline std::optional<core::SoundHeader> getSoundHeader() const { return m_sound; }
            inline std::shared_ptr<sf::Sound> getSound() const 
            { 
                if (!m_sound.has_value())
                    return nullptr;
  
                return m_sound.value().sound; 
            } 
        private:
            std::optional<core::SoundHeader> m_sound;
    };
} 