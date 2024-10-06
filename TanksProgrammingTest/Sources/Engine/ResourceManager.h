#pragma once

#include <string>
#include <map>
#include <memory>
#include <SDL_render.h>
#include <nlohmann/json.hpp>

namespace Engine
{
    using namespace std;

    enum class ResourceType : unsigned int
    {
        Entity,
        Scene
    };

    class Entity;
    class EntityComponent;

    class ResourceManager
    {
    public:
        ResourceManager() = delete;
        ResourceManager(string Path);

        void LoadResources();
        const nlohmann::json& GetJsonConfig(string Name, enum ResourceType Type);
        void RegisterComponent(string Type, EntityComponent* Component);
        const EntityComponent* GetComponentPrototypeByName(string Name);
        unique_ptr<Entity> CreateEntityFromDataTemplate(string Name);
        std::shared_ptr<SDL_Texture> GetOrLoadTexture(string Path);
        void ReleaseTexture(string Path);
        
    private:
        void LoadResourcesFromFolder(string Folder, map<string, nlohmann::json>& MapContainer) const;
        static std::shared_ptr<SDL_Texture> LoadTexture(string Path);
      


        string m_Path;
        map<string, nlohmann::json> m_Entities;
        map<string, nlohmann::json> m_Scenes;
        map<string, EntityComponent*> m_ComponentsPrototypes;
        map<string, shared_ptr<SDL_Texture>> m_Textures;
    };
}
