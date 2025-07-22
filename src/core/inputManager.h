#pragma once
#include "common.h"
#include "inputString.h"

#include <functional>
#include <map>

namespace dmsh::core
{
    enum class InputListenerType
    {
        KeyPressed,
        KeyReleased,
        KeyHold,
        Wheel,
        MouseMoved,
        Unknown = -1,  
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
            Joystick,
            Unknown = -1,  
        };

        struct Listener 
        {
            InputDevice device;
            InputListenerType type;
            KeyCode keyCode;
            MouseButtons mouseButton;
            JoystickAxis joystickAxis;
            std::function<void()> callback;
            bool stayActive; // key hold
        };

        public:
            InputManager() { }
            ~InputManager();
            void addListener(const std::string& name, const InputListenerType& type, const KeyCode& key, 
                const std::function<void()>& callback);

            void addListener(const std::string& name, const InputListenerType& type, const MouseButtons& button, 
                const std::function<void()>& callback);

            void addListener(const std::string& name, const InputListenerType& type, const JoystickAxis& axis, 
                const std::function<void()>& callback);

            void removeListener(const std::string& name);
            
            void update();
            void process(const sf::Event& event);
            
            bool isKeyDown(const KeyCode& key) const;
            bool isKeyReleased(const KeyCode& key) const;

            inline std::string toString() const
            {
                std::stringstream stream;
                static const auto getStateName = [](const InputListenerType& type)
                {
                    switch (type)
                    {                  
                    case InputListenerType::KeyPressed:
                        return "KeyPressed";
                    case InputListenerType::KeyReleased:
                        return "KeyReleased";
                    case InputListenerType::KeyHold:
                        return "KeyHold";
                    case InputListenerType::MouseMoved:
                        return "MouseMoved";
                    case InputListenerType::Wheel:
                        return "Wheel";
                    default:
                        return "Unknown";
                    }
                };

                for (const auto& [key, listener] : m_listeners)
                {
                    switch (listener.device)
                    {
                        case InputDevice::Keyboard:
                            stream << key << " | Keyboard | Key: " << 
                                (listener.keyCode == KeyCode::Unknown ? "Unknown" : keyString[static_cast<std::int32_t>(listener.keyCode)]) <<
                                " | Type:" << getStateName(listener.type) <<
                                "\n";
                            break;
                        case InputDevice::Mouse:
                            stream << key << " | Mouse | Button: " <<
                                mouseString[static_cast<std::int32_t>(listener.mouseButton)] <<
                                " | Type:" << getStateName(listener.type) <<
                                "\n";
                            break;
                        case InputDevice::Joystick:
                            stream << key << " | Joystick | Not implemented " << 
                            "\n";
                            break;
                        default:
                            stream << key << " | Unknown Device | " << 
                            "\n";
                            break;
                    }
                } 

                return stream.str();
            }
        private:
            std::map<std::string, InputManager::Listener> m_listeners;
    };
}