#pragma once
#include "common.h"
#include "text.h"

namespace dmsh::core
{
    class Button : public Text
    {
        public:
            Button() 
            { 

            }

            virtual ~Button() 
            { 

            }  

            virtual void onStart() override
            {
                Text::onStart();
                auto owner = getOwner();
                auto drawable = owner->createComponent<Drawable>();
                m_shape = drawable->create<sf::RectangleShape>();

                // Apply transform callbacks for drawable 
                auto transform = owner->getTransform();
                transform->Signals.onPositionChanged.connect(&Drawable::updatePositionForTransformable, drawable.get());
                transform->Signals.onRotationChanged.connect(&Drawable::updateRotationForTransformable, drawable.get());
                transform->Signals.onScaleChanged.connect(&Drawable::updateScaleForTransformable, drawable.get());
                transform->invokeCallbacks(); 
            }
            
        private:
            sf::RectangleShape m_shape;
    };
}