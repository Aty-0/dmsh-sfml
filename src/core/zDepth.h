#pragma once 
#include "common.h"

namespace dmsh::core
{
    struct zDepth
    {
        static constexpr auto DEBUG_LAYER = 1024;
        static constexpr auto UI_LAYER = (DEBUG_LAYER - 1);
        
        zDepth() : m_zDepth(0) { }
        ~zDepth() { }

        void setZDepth(std::uint16_t z, bool rebuild = false);
        inline std::uint16_t getZDepth() { return m_zDepth; }
        private:
            std::uint16_t m_zDepth;
    };
}