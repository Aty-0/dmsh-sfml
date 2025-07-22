#include "window.h"
#include "logger.h"

namespace dmsh::core
{
    void Window::create(std::int32_t w, std::int32_t h, const char* name)
    {
        DMSH_DEBUG("Create window");
        sf::VideoMode videoMode = { };
        videoMode.size.x = w; 
        videoMode.size.y = h;
        videoMode.bitsPerPixel  = 32;
        DMSH_ASSERT(videoMode.isValid(), "video mode is not valid");
        constexpr auto defaultWindowFlags = sf::Style::Titlebar | sf::Style::Close;
        m_renderWindow = sf::RenderWindow(videoMode, name, defaultWindowFlags);
        m_renderWindow.setMouseCursorVisible(true);
        m_renderWindow.setKeyRepeatEnabled(false);
		//m_renderWindow.setVerticalSyncEnabled(true);
		//m_renderWindow.setFramerateLimit(60);
    }
}