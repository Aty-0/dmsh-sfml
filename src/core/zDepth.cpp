#include "zDepth.h"
#include "sceneManager.h"

namespace dmsh::core
{
    void zDepth::setZDepth(std::uint16_t z, bool rebuild)
    {
        m_zDepth = z;
        if (rebuild)
        {
            static const auto sceneManager = SceneManager::getInstance();
            sceneManager->rebuildZOrdering();
        }
    }
}