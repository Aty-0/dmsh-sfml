#pragma once
#include "../core/singleton.h"
#include "../core/sceneManager.h"

namespace dmsh::game
{
    class NodeEditor;
    class StageLoader : public core::Singleton<StageLoader>
    {
        public:
            void loadStage(std::string_view name);
            void loadPattern(std::string_view name);

            // FIXME: meh
            inline void setNodeEditorComponentPtr(std::shared_ptr<NodeEditor> nodeEditor) { m_nodeEditor = nodeEditor; }
        private:
            std::shared_ptr<NodeEditor> m_nodeEditor;
    };
}