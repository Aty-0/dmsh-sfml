#pragma once 
#include "common.h"

#include <unordered_set>
 
// Declare new viewspace getter 
#define DECLARE_VIEW_SPACE(Name, Priority) \
    inline static dmsh::core::ViewSpace& getViewSpace##Name() { \
        static auto view = dmsh::core::ViewSpace { #Name, Priority }; \
        return view; \
    } \

namespace dmsh::core
{
    // View space is a struct which show which camera (space) we will be use 
    class ViewSpace 
    {        
        public: 
            explicit ViewSpace(std::string_view name, std::uint8_t priority = 0) : 
                    m_name(name), 
                    m_priority(priority)
            {

            }

            inline void setView(const sf::View& view) { m_view = view; }

            inline sf::View& getView() { return m_view; }
            inline const sf::View& getView() const { return m_view; }
            inline std::string_view getName() const { return m_name; }
            
        private:
            std::string m_name;
            sf::View m_view;      
            // TODO: Do we need this ?  
            std::uint8_t m_priority;
    };
    
    // Decalring engine view spaces    
    DECLARE_VIEW_SPACE(UI, 0);
    DECLARE_VIEW_SPACE(Game, 1);
}