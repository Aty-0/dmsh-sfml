#pragma once
#include "common.h"

namespace dmsh::core
{
    // TODO: If needed add get set w,h,x,y and name 
    class Window  : public Singleton<Window>
    {
        public:
            Window() { }
            void create(std::int32_t w, std::int32_t h, const char* name);
            inline sf::RenderWindow& getWindow() { return m_renderWindow; }
            inline bool isOpen() const { return m_renderWindow.isOpen(); }
            inline void close() { m_renderWindow.close(); }
            inline sf::Vector2i getMousePosition() const { return sf::Mouse::getPosition(m_renderWindow); }
            
            private:
                sf::RenderWindow m_renderWindow;        
    };
}