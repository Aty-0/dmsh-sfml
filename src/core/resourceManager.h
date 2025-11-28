#pragma once
#include "common.h"
#include "logger.h"

#include <format>
#include <unordered_map>
#include <optional>
#include <fstream>

namespace dmsh::core 
{
    static constexpr std::string_view RESOURCE_FOLDER_NAME = "gamedata";
    static constexpr std::string_view DEFAULT_RESOURCE_TABLE_NAME = "reg.json";

    enum class ResourceTypes
    {
        Texture,
        Sound,
        Music,
        Font,
        Json,
        Xml, 
    };

    struct ResourceMeta 
    {
        std::string path;
        std::string name;
        ResourceTypes type;
    };

    template<ResourceTypes Type>
    struct ResourceTrait { };
    
    struct IResource
    {
        virtual ~IResource() 
        {
            m_meta.release();
            unload();
        }

        virtual bool load(void* data, std::size_t size) = 0;
        virtual bool unload() { return true; }

        inline void setMeta(const ResourceMeta& meta) 
        { 
            if (!m_meta) 
                m_meta = std::make_unique<ResourceMeta>();
            
            *m_meta = meta; 
        }

        inline ResourceMeta getMeta() const 
        {
            DMSH_ASSERT(m_meta, "No meta in this resource");
            return *m_meta; 
        }

        protected: 
            std::unique_ptr<ResourceMeta> m_meta;
    };

    template<typename T>
    struct ResourceHandle
    {
        ResourceHandle()
        {
            m_handle = std::make_shared<T>();
        }

        inline std::shared_ptr<T> getHandle() const { return m_handle; } 
        
        protected:
            std::shared_ptr<T> m_handle = nullptr;
    };

    class ResourceManager : public Singleton<ResourceManager>
    {
        public:
            ResourceManager() = default;
            ~ResourceManager();
            void init();

            // Get raw file from resource folder 
            std::optional<std::ifstream> getFile(std::string_view path);        

            // Check file on exist in resource folder 
            inline bool checkFileOnExist(std::string_view path) 
            {
                const auto finalPath = std::format("{}/{}", RESOURCE_FOLDER_NAME, path); 
                return !std::filesystem::is_directory(finalPath) && std::filesystem::exists(finalPath); 
            }

        private: 
            std::unordered_map<std::string_view, std::shared_ptr<IResource>> m_resources;
            struct FileData
            {
                void* data;
                std::size_t size;
            };

            std::optional<FileData> read(std::string_view path, std::string_view name);
        public: 
            template<ResourceTypes Type>
            inline std::shared_ptr<ResourceTrait<Type>> get(std::string_view name) const 
            {
                // Check on key exist 
                if (m_resources.find(name) == m_resources.end()) 
                {
                    DMSH_DEBUG("Can't get resource, invalid key");
                    return nullptr;
                }

                const auto resource = m_resources.at(name);
                // Avoid nullptr resource 
                if (!resource)
                    return nullptr;

                return std::dynamic_pointer_cast<ResourceTrait<Type>>(resource);
            }

            // Load resource from resource folder
            template<ResourceTypes Type>
            inline void load(std::string_view path, std::string_view name) 
            {
                auto dataResult = read(path, name);
                if (!dataResult.has_value())
                {
                    return;
                }

                const auto data = dataResult.value();
                const auto meta = ResourceMeta { std::string(path), std::string(name), Type };
                const auto resource = std::make_shared<ResourceTrait<Type>>();
                const auto loadResult = resource->load(data.data, data.size);
                if (!loadResult)
                {
                    // TODO: Description
                    DMSH_ERROR("Failed to load resource");
                    // TODO: Use default if failed, if default failed we are crashing the engine
                    return;
                }
                m_resources.insert({ name, std::move(resource)});
            } 

    };

    template<>
    struct ResourceTrait<ResourceTypes::Texture> : public IResource, public ResourceHandle<sf::Texture> 
    {
        virtual bool load(void* data, std::size_t size) override 
        {            
            return m_handle->loadFromMemory(data, size);
        }

        virtual bool unload() override
        {
            m_handle.reset();
            return true;
        }
    };

    template<>
    struct ResourceTrait<ResourceTypes::Font> : public IResource, public ResourceHandle<sf::Font> 
    {
        virtual bool load(void* data, std::size_t size) override 
        {            
            return m_handle->openFromMemory(data, size);
        }

        virtual bool unload() override
        {
            m_handle.reset();
            return true;
        }
    };
  
    template<>
    struct ResourceTrait<ResourceTypes::Sound> : public IResource, public ResourceHandle<sf::SoundBuffer> 
    {
        virtual bool load(void* data, std::size_t size) override 
        {                    
            return m_handle->loadFromMemory(data, size);
        }

        virtual bool unload() override
        {
            m_handle.reset();
            return true;
        }
    };

    template<>
    struct ResourceTrait<ResourceTypes::Json> : public IResource, public ResourceHandle<nlohmann::json> 
    {
        using json = nlohmann::json;

        virtual bool load(void* data, std::size_t size) override 
        {            
            const auto& parsed = json::parse(std::string(static_cast<const char*>(data), size)); 
            *m_handle = parsed;
            
            return true;
        }

        virtual bool unload() override
        {
            m_handle.reset();
            return true;
        }

        inline void dump()
        {
            DMSH_DEBUG(m_handle->dump().c_str());
        }
    };
}
