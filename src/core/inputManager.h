#pragma once
#include "common.h"
#include <functional>
#include <vector>

namespace dmsh::core
{
    enum class InputListenerType
    {
        KeyPressed,
        KeyReleased,
        KeyHold,
        Wheel,
        MouseMoved,
    };
    
    using KeyCode = sf::Keyboard::Key;
    using MouseButtons = sf::Mouse::Button;
    using JoystickAxis = sf::Joystick::Axis;

    class InputManager : public Singleton<InputManager>
    {
        enum class InputDevice
        {
            Mouse,
            Keyboard,
            Joystick  
        };

        struct Listener 
        {
            InputDevice device;
            InputListenerType type;
            KeyCode keyCode;
            MouseButtons mouseButtons;
            JoystickAxis joystickAxis;
            std::function<void()> callback;
            bool stayActive; // key hold
        };

        public:
            InputManager() { }
            ~InputManager();
            void addListener(const InputListenerType& type, const KeyCode& key, 
                const std::function<void()>& callback);

            void addListener(const InputListenerType& type, const MouseButtons& button, 
                const std::function<void()>& callback);

            void addListener(const InputListenerType& type, const JoystickAxis& axis, 
                const std::function<void()>& callback);

            void removeListener(const KeyCode& key);
            void removeListener(const MouseButtons& button);
            void removeListener(const JoystickAxis& axis);
            
            void update();
            void process(const sf::Event& event);
            
            bool isKeyDown(const KeyCode& key) const;
            bool isKeyReleased(const KeyCode& key) const;
              
        private:
            std::vector<InputManager::Listener> m_listeners;
    };
}