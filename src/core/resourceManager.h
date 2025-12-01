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
        Shader,
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
        }

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

            struct FileData
            {
                void* data;
                std::size_t size;
            };
        private: 
            std::unordered_map<std::string_view, std::shared_ptr<IResource>> m_resources;
            std::optional<ResourceManager::FileData> read(std::string_view path, std::string_view name);
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

            // Load several files as one resource             
            template<ResourceTypes Type, typename... FilePath, typename... Args>
            inline void load(std::string_view name, std::tuple<FilePath...> paths, Args&&... args)
            {   
                constexpr std::size_t size = sizeof...(FilePath);
                const auto arrayPaths = std::apply([](auto&&... e) { return std::array { std::forward<decltype(e)>(e) ... }; }, paths);

                std::array<FileData, size> datas = { };                             
                for (std::size_t i = 0; i < size; ++i)
                {
                    const auto path = arrayPaths[i];
                    const auto dataResult = read(path, name);
                    if (!dataResult.has_value())
                    {
                        DMSH_ERROR("Failed to load file for resource with several files");
                        return;
                    }

                    datas[i] = std::move(dataResult.value());
                }
                
                // TODO: Pass to meta many paths
                const auto meta = ResourceMeta { "TODO", std::string(name), Type };
                const auto resource = std::make_shared<ResourceTrait<Type>>();
                const auto loadResult = resource->load<size>(std::move(datas), std::forward<Args>(args)...);
                if (!loadResult)
                {
                    // TODO: Description
                    DMSH_ERROR("Failed to load resource");
                    // TODO: Use default if failed, if default failed we are crashing the engine
                    return;
                }

                m_resources.insert({ name, std::move(resource)});
            }

            // Load resource from resource folder
            template<ResourceTypes Type, typename... Args>
            inline void load(std::string_view path, std::string_view name, Args&&... args) 
            {
                auto dataResult = read(path, name);
                if (!dataResult.has_value())
                {
                    return;
                }

                const auto data = dataResult.value();
                const auto meta = ResourceMeta { std::string(path), std::string(name), Type };
                const auto resource = std::make_shared<ResourceTrait<Type>>();
                const auto loadResult = resource->load(data.data, data.size, std::forward<Args>(args)...);
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
        inline bool load(void* data, std::size_t size) 
        {            
            return m_handle->loadFromMemory(data, size);
        }
    };

    template<>
    struct ResourceTrait<ResourceTypes::Font> : public IResource, public ResourceHandle<sf::Font> 
    {
        inline bool load(void* data, std::size_t size) 
        {            
            return m_handle->openFromMemory(data, size);
        }

    };
  
    template<>
    struct ResourceTrait<ResourceTypes::Sound> : public IResource, public ResourceHandle<sf::SoundBuffer> 
    {
        inline bool load(void* data, std::size_t size) 
        {                    
            return m_handle->loadFromMemory(data, size);
        }
    };
    
    template<>
    struct ResourceTrait<ResourceTypes::Shader> : public IResource, public ResourceHandle<sf::Shader> 
    {        
        template<std::size_t Size, typename... Args>
        inline bool load(std::array<ResourceManager::FileData, Size>&& datas, Args&&... args) 
        {     
            static_assert(Size != 0, "Invalid size");

            std::array<std::string_view, Size> stringData;
            for (std::size_t i = 0; i < Size; ++i)
            {
                if (datas[i].data)
                    stringData[i] = std::string_view(static_cast<char*>(datas[i].data), datas[i].size);
                else                 
                    return false;                
            }

            if constexpr (Size == 1)            
                return m_handle->loadFromMemory(stringData[0]);            
            else if constexpr (Size == 2)            
                return m_handle->loadFromMemory(stringData[0], stringData[1]);            
            else if constexpr (Size == 3)            
                return m_handle->loadFromMemory(stringData[0], stringData[1], stringData[2]);            

            DMSH_ASSERT(false, "Unreachable code...");
            return false;
        }

        template<typename... Args>
        inline bool load(void* data, std::size_t size, Args&&... args) 
        {     
            const auto extractedArgs = std::forward_as_tuple(std::forward<Args>(args)...);
            const sf::Shader::Type type = std::get<0>(extractedArgs);
            return m_handle->loadFromMemory(std::string_view(static_cast<char*>(data), size), type);
        }
    };
    
    template<>
    struct ResourceTrait<ResourceTypes::Json> : public IResource, public ResourceHandle<nlohmann::json> 
    {
        using json = nlohmann::json;

        inline bool load(void* data, std::size_t size)
        {            
            const auto& parsed = json::parse(std::string(static_cast<const char*>(data), size)); 
            *m_handle = parsed;
            
            return true;
        }

        inline void dump()
        {
            DMSH_DEBUG(m_handle->dump().c_str());
        }
    };
}
