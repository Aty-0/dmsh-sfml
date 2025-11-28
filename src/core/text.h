#pragma once
#include "common.h"
#include "component.h"
#include "gameObject.h"
#include "resourceManager.h"

namespace dmsh::core
{
    class Text : public Component
    {
        public:
            Text() 
            { 

            }

            virtual ~Text() 
            { 
                m_drawableText = nullptr;
            }  

            virtual void onStart() override
            {
                static const auto font = ResourceManager::getInstance()->get<ResourceTypes::Font>("immortal");
                DMSH_ASSERT(font, "Default font is not found");

                auto owner = getOwner();
                owner->setZDepth(zDepth::UI_LAYER, true);

                m_drawableText = &owner->getDrawable()->create<sf::Text>(*font->getHandle());            
                
                owner->setViewSpace(core::getViewSpaceUI());
            }
            
            inline std::string getText() const 
            {
                if (m_drawableText == nullptr)
                    return "";

                return m_drawableText->getString();
            }

            template<typename... Args>
            inline void setText(std::string_view text, Args&&... args)
            {
                if (m_drawableText != nullptr)
                {
                    m_drawableText->setString(std::vformat(text, std::make_format_args(args...)));
                    onTextChanged(text);
                }
                
            }

            inline void setText(std::string_view text)
            {
                if (m_drawableText != nullptr)
                {
                    m_drawableText->setString(std::string{ text });
                    onTextChanged(text);
                }
            }

            inline virtual void setFillColor(const sf::Color& color)
            {
                if (m_drawableText != nullptr)
                    m_drawableText->setFillColor(color);
            }

            inline virtual void setOutlineColor(const sf::Color& color)
            {
                if (m_drawableText != nullptr)
                    m_drawableText->setOutlineColor(color);
            }

            inline virtual void setSize(std::uint32_t size)
            {
                if (m_drawableText != nullptr)
                    m_drawableText->setCharacterSize(size);
            }

            inline virtual void setLetterSpacing(float spacing)
            {
                if (m_drawableText != nullptr)
                    m_drawableText->setLetterSpacing(spacing);
            }

            inline virtual void setLineSpacing(float spacing)
            {
                if (m_drawableText != nullptr)
                    m_drawableText->setLineSpacing(spacing);
            }

            inline virtual void setOutlineThickness(float thickness)
            {
                if (m_drawableText != nullptr)
                    m_drawableText->setOutlineThickness(thickness);
            }
                
        private:
            sf::Text* m_drawableText;
        public:
            // Args: std::string_view text -> New text 
            sigslot::signal<std::string_view> onTextChanged;
    };
}