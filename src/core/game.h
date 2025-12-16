#pragma once
#include "common.h"
#include "sandbox.h"

namespace dmsh::core
{
    class Window;
    class Game : public Singleton<Game> 
    {
        public:
            Game() { }

            // Register sandbox
            template<typename T>
            void run();

        private:
            void runEngine();
            void runLoop(Window* window);
            void poolEvents(sf::RenderWindow& window);
            void onRender(sf::RenderWindow& window);
            void onUpdate(float delta);
            void onClose();
            void onResize();
            void setupViewSpaces(const sf::Vector2u& size);

            std::shared_ptr<ISandbox> m_sandbox;
    };
    
    template<typename T>
    inline void Game::run()
    {
        static_assert(std::is_base_of<ISandbox, T>::value, "class must be based on ISandbox");      
        m_sandbox = std::make_shared<T>();
        runEngine();
    }
}