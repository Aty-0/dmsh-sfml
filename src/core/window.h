#pragma once
#include "common.h"

namespace dmsh::core
{
    // TODO: If needed add get set w,h,x,y and name 
    class Window  : public Singleton<Window>
    {
        public:
            Window() { }
            void create(std::int32_t w, std::int32_t h, std::string_view name);

            inline void close() { m_renderWindow.close(); }

            inline sf::Vector2u getSize() const { return m_renderWindow.getSize(); }
            inline sf::Vector2i getPosition() const { return m_renderWindow.getPosition(); }
            inline std::uint32_t getWidth() const { return m_renderWindow.getSize().x; }
            inline std::uint32_t getHeight() const { return m_renderWindow.getSize().y; }
            inline std::int32_t getPositionX() const { return m_renderWindow.getPosition().x; }
            inline std::int32_t getPositionY() const { return m_renderWindow.getPosition().y; }
            inline sf::Vector2i getMousePosition() const { return sf::Mouse::getPosition(m_renderWindow); }
            inline sf::RenderWindow& getWindow() { return m_renderWindow; }
            inline bool isOpen() const { return m_renderWindow.isOpen(); }

            
            private:
                sf::RenderWindow m_renderWindow;        
    };
}