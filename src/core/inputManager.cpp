#include "inputManager.h"
#include "logger.h"
#include <ranges>

namespace dmsh::core
{
    InputManager::~InputManager()
    {
        m_listeners.clear();
        m_listeners.shrink_to_fit();
    }

    void InputManager::addListener(const InputListenerType& type, const KeyCode& key, 
        const std::function<void()>& callback)
    {
        auto listener = Listener();
        listener.device = InputDevice::Keyboard;
        listener.type = type;
        listener.keyCode = key;
        listener.callback = callback;
        m_listeners.push_back(listener);
    }

    void InputManager::addListener(const InputListenerType& type, const MouseButtons& button, 
        const std::function<void()>& callback)
    {
        auto listener = Listener();
        listener.device = InputDevice::Mouse;
        listener.type = type;
        listener.mouseButtons = button;
        listener.callback = callback;
        m_listeners.push_back(listener);
    }
    
    void InputManager::addListener(const  InputListenerType& type, const JoystickAxis& axis, 
        const std::function<void()>& callback)
    {
        auto listener = Listener();
        listener.device = InputDevice::Joystick;
        listener.type = type;
        listener.joystickAxis = axis;
        listener.callback = callback;
        m_listeners.push_back(listener);
    }

    void InputManager::removeListener(const KeyCode& key)
    {
        std::erase_if(m_listeners, [&](auto elem) { 
            return elem.keyCode == key;
        });
    }

    void InputManager::removeListener(const MouseButtons& button)
    {
        std::erase_if(m_listeners, [&](auto elem) { 
            return elem.mouseButtons == button;
        });
    }

    void InputManager::removeListener(const JoystickAxis& axis)
    {
        std::erase_if(m_listeners, [&](auto elem) { 
            return elem.joystickAxis == axis;
        });
    }
    
    void InputManager::update()
    {
        std::ranges::for_each(m_listeners, [&](auto& listener) {                        
            if (listener.stayActive)
                listener.callback();     
        });
    }

    bool InputManager::isKeyDown(const KeyCode& key) const
    {
        return false; 
    }
    
    bool InputManager::isKeyReleased(const KeyCode& key) const
    {        
        return false; 
    }
              
    void InputManager::process(const sf::Event& event)
    {
        std::ranges::for_each(m_listeners, [&](auto& listener) {                        
            switch (listener.device)
            {
                case InputDevice::Keyboard:
                {
                    switch (listener.type)
                    {
                        case InputListenerType::KeyPressed:   
                        {
                            auto key = event.getIf<sf::Event::KeyPressed>();
                            if (key != nullptr && key->code == listener.keyCode && listener.callback != nullptr)
                                listener.callback();                 
                            break;                
                        }
                        case InputListenerType::KeyReleased:
                        {
                            auto key = event.getIf<sf::Event::KeyReleased>();
                            if (key != nullptr && key->code == listener.keyCode && listener.callback != nullptr)
                                listener.callback();
                            break;                                    
                        }
                        case InputListenerType::KeyHold:
                        {
                            if (listener.callback == nullptr)
                                break;

                            auto keyPressed = event.getIf<sf::Event::KeyPressed>();
                            auto keyReleased = event.getIf<sf::Event::KeyReleased>();
                            
                            if (keyPressed != nullptr && keyPressed->code == listener.keyCode)
                                listener.stayActive = true;
                            else if (listener.stayActive && keyReleased != nullptr && keyReleased->code == listener.keyCode)
                                listener.stayActive = false;
                         
                            break;                
                        }
                        default:
                            DMSH_ASSERT(false, "Unknown input state");
                            break;
                    }
                    break;
                }
                case InputDevice::Mouse:
                {
                    switch (listener.type)
                    {
                        case InputListenerType::MouseMoved:
                        {
                            auto move = event.getIf<sf::Event::MouseMoved>();
                            if (move != nullptr && listener.callback != nullptr)
                                listener.callback();                 
                            break;
                        }   
                        case InputListenerType::Wheel:
                        {
                            auto scroll = event.getIf<sf::Event::MouseWheelScrolled>();
                            if (scroll != nullptr && listener.callback != nullptr)
                                listener.callback();                 
                            break;
                        }
                        case InputListenerType::KeyPressed:   
                        {
                            auto button = event.getIf<sf::Event::MouseButtonPressed>();
                            if (button != nullptr && button->button == listener.mouseButtons && listener.callback != nullptr)
                                listener.callback();                 
                            break;                
                        }
                        case InputListenerType::KeyReleased:
                        {
                            auto button = event.getIf<sf::Event::MouseButtonReleased>();
                            if (button != nullptr && button->button == listener.mouseButtons && listener.callback != nullptr)
                                listener.callback();                 
                            break;                                    
                        }
                        default:
                            DMSH_ASSERT(false, "Unknown input state");
                            break;
                    }
                    break;
                }
                case InputDevice::Joystick:
                {
                    // TODO: joystick index
                    if (!sf::Joystick::isConnected(0))
                        break;
                    break;            
                }
                default:
                    DMSH_ASSERT(false, "Unknown device");
                    break;
            }
        });
    }
}