#pragma once
#include "common.h"

namespace dmsh::core
{
    class ISandbox
    {
        public:
            virtual void onStart() = 0;
            virtual void onDestroy() = 0;
            virtual void onUpdate(float delta) = 0;
            virtual void onRender(sf::RenderTarget& target) = 0;
            virtual void onInput() = 0;
            virtual void onGui() = 0;
    };
}