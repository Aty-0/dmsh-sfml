#include "inputManager.h"
#include "logger.h"
#include <ranges>

namespace dmsh::core
{
    InputManager::~InputManager()
    {
        m_listeners.clear();
    }

    void InputManager::addListener(const std::string& name, const InputListenerType& type, const KeyCode& key)
    {
        auto listener = Listener();
        listener.device = InputDevice::Keyboard;
        listener.type = type;
        listener.keyCode = key;
        m_listeners.insert({name, listener});
    }
    
    void InputManager::addListener(const std::string& name, const InputListenerType& type, const MouseButtons& button)
    {
        auto listener = Listener();
        listener.device = InputDevice::Mouse;
        listener.type = type;
        listener.mouseButton = button;
        m_listeners.insert({name, listener});
    }

    void InputManager::addListener(const std::string& name, const InputListenerType& type, const JoystickAxis& axis)
    {
        auto listener = Listener();
        listener.device = InputDevice::Joystick;
        listener.type = type;
        listener.joystickAxis = axis;
        m_listeners.insert({name, listener});
    }

       
    void InputManager::addListener(const std::string& name, const InputListenerType& type, const KeyCode& key, 
        const std::function<void()>& callback)
    {
        auto listener = Listener();
        listener.device = InputDevice::Keyboard;
        listener.type = type;
        listener.keyCode = key;
        listener.callback = callback;
        m_listeners.insert({name, listener});
    }

    void InputManager::addListener(const std::string& name, const InputListenerType& type, const MouseButtons& button, 
        const std::function<void()>& callback)
    {
        auto listener = Listener();
        listener.device = InputDevice::Mouse;
        listener.type = type;
        listener.mouseButton = button;
        listener.callback = callback;
        m_listeners.insert({name, listener});
    }
    
    void InputManager::addListener(const std::string& name, const InputListenerType& type, const JoystickAxis& axis, 
        const std::function<void()>& callback)
    {
        auto listener = Listener();
        listener.device = InputDevice::Joystick;
        listener.type = type;
        listener.joystickAxis = axis;
        listener.callback = callback;
        m_listeners.insert({name, listener});
    }

    void InputManager::removeListener(const std::string& name)
    {
        m_listeners.erase(name);
    }

    void InputManager::update()
    {
        for (const auto& [key, listener] : m_listeners)
        {
            if (listener.stayActive && listener.callback != nullptr)
                listener.callback();     
        }                     
    }

    bool InputManager::isListenerActive(const std::string& name) 
    {
        auto listener = m_listeners.find(name);
        if (listener == m_listeners.end())
            return false;

        const bool result = listener->second.stayActive || listener->second.isActive;
        if (result && !listener->second.stayActive) 
        {
            listener->second.isActive = false;
        }

        return result;
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
        for (auto& [key, listener] : m_listeners)
        {
            switch (listener.device)
            {
                case InputDevice::Keyboard:
                {
                    switch (listener.type)
                    {
                        case InputListenerType::KeyPressed:   
                        {
                            auto key = event.getIf<sf::Event::KeyPressed>();
                            if (key != nullptr && key->code == listener.keyCode)
                            {
                                listener.isActive = true;
                                if (listener.callback != nullptr)
                                    listener.callback();                 
                            }
                            break;                
                        }
                        case InputListenerType::KeyReleased:
                        {
                            auto key = event.getIf<sf::Event::KeyReleased>();
                            if (key != nullptr && key->code == listener.keyCode)
                            {
                                listener.isActive = true;
                                if (listener.callback != nullptr)
                                    listener.callback();
                            }
                            break;                                    
                        }
                        case InputListenerType::KeyHold:
                        {
                            auto keyPressed = event.getIf<sf::Event::KeyPressed>();
                            auto keyReleased = event.getIf<sf::Event::KeyReleased>();
                            
                            if (keyPressed != nullptr && keyPressed->code == listener.keyCode)
                            {
                                listener.isActive = true;
                                listener.stayActive = true;
                            }
                            else if (listener.stayActive && keyReleased != nullptr && keyReleased->code == listener.keyCode)
                            {
                                listener.isActive = false;
                                listener.stayActive = false;
                            }
                         
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
                            if (move != nullptr)
                            {
                                listener.isActive = true;
                                if (listener.callback != nullptr)
                                    listener.callback();                 
                            }
                            break;
                        }   
                        case InputListenerType::Wheel:
                        {
                            auto scroll = event.getIf<sf::Event::MouseWheelScrolled>();
                            if (scroll != nullptr)
                            {
                                listener.isActive = true;
                                if (listener.callback != nullptr)
                                    listener.callback();                 
                            }
                            break;
                        }
                        case InputListenerType::KeyPressed:   
                        {
                            auto button = event.getIf<sf::Event::MouseButtonPressed>();
                            if (button != nullptr && button->button == listener.mouseButton)
                            {
                                listener.isActive = true;

                                if (listener.callback != nullptr)
                                    listener.callback();                 
                            }
                            break;                
                        }
                        case InputListenerType::KeyReleased:
                        {
                            auto button = event.getIf<sf::Event::MouseButtonReleased>();
                            if (button != nullptr && button->button == listener.mouseButton)
                            {
                                listener.isActive = true;
    
                                if (listener.callback != nullptr)
                                    listener.callback();                 
                            }
                            break;                                    
                        }
                        case InputListenerType::KeyHold:
                        {
                            auto buttonPressed = event.getIf<sf::Event::MouseButtonPressed>();
                            auto buttonReleased = event.getIf<sf::Event::MouseButtonReleased>();
                            
                            if (buttonPressed != nullptr && buttonPressed->button == listener.mouseButton)
                            {
                                listener.isActive = true;
                                listener.stayActive = true;
                            }
                            else if (listener.stayActive && buttonReleased != nullptr && buttonReleased->button == listener.mouseButton)
                            {
                                listener.isActive = false;
                                listener.stayActive = false;
                            }
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
        }
    }
}