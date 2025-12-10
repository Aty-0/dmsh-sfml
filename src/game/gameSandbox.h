#pragma once
#include "../core/sandbox.h"

namespace dmsh::game
{
    class GameSandbox : public core::ISandbox
    {
        public:
            virtual void onStart() override;
            virtual void onDestroy() override;
            virtual void onUpdate(float delta) override;
            virtual void onRender(sf::RenderTarget& target) override;
            virtual void onInput() override;
            virtual void onGui() override;
    };

    class EditorSandbox : public core::ISandbox
    {
        public:
            virtual void onStart() override;
            virtual void onDestroy() override;
            virtual void onUpdate(float delta) override;
            virtual void onRender(sf::RenderTarget& target) override;
            virtual void onInput() override;
            virtual void onGui() override;
    };
}