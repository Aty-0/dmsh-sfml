#include "stageLoader.h"
#include "nodeEditor.h"
#include "../core/resourceManager.h"

namespace dmsh::game 
{
    static const auto resourceManager = core::ResourceManager::getInstance();

    void StageLoader::loadStage(std::string_view name)
    {
        // TODO: Implement stage system
    }
    
    void StageLoader::loadPattern(std::string_view name)
    {
        const auto path = std::format("patterns/{}.json", name); 
        resourceManager->load<core::ResourceTypes::Json>(path, name);
        const auto resource = resourceManager->get<core::ResourceTypes::Json>(name);
        if (resource == nullptr)
        {
            DMSH_ERROR("Failed to load pattern %s", name.data());
            return;
        }

        m_nodeEditor->clear();

        const auto json = *resource->getHandle();
        auto patterns = json[1].get<std::vector<JsonPattern>>();
  
        for (const auto pattern : patterns)
        {
            // Make new pattern 
            m_nodeEditor->createNewPattern();
            // Then make nodes 
            for (const auto node : pattern.nodes)
            {
                // TODO: Load it just for game if we are not in editor mode 
                m_nodeEditor->createNodeByJsonNode(node);
            }
        }
    }
}