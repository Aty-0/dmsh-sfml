#include "resourceManager.h"

#include <filesystem>

namespace dmsh::core 
{
    ResourceManager::ResourceManager()
    {

    }

    ResourceManager::~ResourceManager()
    {

    }

    void ResourceManager::init() 
    {
        // Check basics, resource folder first and then resource table 
        const auto resourceFolderIsExist = std::filesystem::is_directory(RESOURCE_FOLDER_NAME) && std::filesystem::exists(RESOURCE_FOLDER_NAME);
        DMSH_ASSERT(resourceFolderIsExist, "Resource folder is invalid");
        DMSH_ASSERT(checkFileOnExist(DEFAULT_RESOURCE_TABLE_NAME), "Resource table file is invalid");
        // TODO: Load resource table and then load other resources 
    }

    std::optional<ResourceManager::FileData> ResourceManager::read(std::string_view path, std::string_view name)
    {
        // TODO: Do we need to check file on exist ? 
        DMSH_DEBUG("Load file %s", path.data());

        auto result = getFile(path);
        if (!result.has_value())
            return std::nullopt;

        auto& file = result.value();
        std::size_t size = file.tellg();
		file.seekg(0, std::ios::beg);
		
		auto buffer = new char[size + 1];
		buffer[size] = '\0';

		if (!file.read(buffer, size))
        {
            DMSH_ERROR("Couldn't read file.");
            return std::nullopt;
        }

        return ResourceManager::FileData { static_cast<void*>(buffer), size };
    }

    std::optional<std::ifstream> ResourceManager::getFile(std::string_view path)
    {
        const auto finalPath = std::format("{}/{}", RESOURCE_FOLDER_NAME, path); 
        DMSH_DEBUG("Try to get file %s", finalPath.data());
        if (!checkFileOnExist(path)) 
        {
            DMSH_ERROR("File is not found!");
            return std::nullopt;
        }

        std::ifstream file;
        try 
        {
            file.open(finalPath, std::ios::binary | std::ios::ate);
        }
        catch (std::ios_base::failure& e) 
        {
            DMSH_ERROR("Can't open file! Code: %i", e.code().value());            
            return std::nullopt;
        }

        return file;
    } 

    
    std::any get(const ResourceTypes& type, std::string_view name)
    {
        return nullptr;
    }
}
