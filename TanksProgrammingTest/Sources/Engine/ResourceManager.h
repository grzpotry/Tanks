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
        ResourceManager(const string& Path);

        void LoadResources();
        const nlohmann::json& GetJsonConfig(const string& Name, enum ResourceType Type);
        void RegisterComponent(const string& Type, EntityComponent* Component);
        const EntityComponent* GetComponentPrototypeByName(const string& Name);
        shared_ptr<Entity> CreateEntityFromDataTemplate(const string& Name, Entity* const Parent = nullptr);
        std::shared_ptr<SDL_Texture> GetOrLoadTexture(const string& Path);
        void ReleaseTexture(const string& Path);
        
    private:
        void LoadResourcesFromFolder(const string& Folder, map<string, nlohmann::json>& MapContainer) const;
        static std::shared_ptr<SDL_Texture> LoadTexture(const string& Path);
      


        string m_Path;
        map<string, nlohmann::json> m_Entities;
        map<string, nlohmann::json> m_Scenes;
        map<string, EntityComponent*> m_ComponentsPrototypes;
        map<string, shared_ptr<SDL_Texture>> m_Textures;
    };
}
