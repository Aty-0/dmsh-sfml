#pragma once
#include "common.h"

namespace dmsh::core
{
    class Window;
    class Game : public Singleton<Game> 
    {
        public:
            Game() { }
            
            void run();
        private:
            void runLoop(Window* window);
            void poolEvents(sf::RenderWindow& window);
            void onRender(sf::RenderWindow& window);
            void onUpdate(float delta);
            void onClose();
    };
}