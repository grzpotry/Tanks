#pragma once

#include <string>
#include <map>
#include <memory>
#include <SDL_render.h>
#include <nlohmann/json.hpp>

namespace EngineCore
{
    using namespace std;

    enum class ResourceType : unsigned int
    {
        Entity,
        Scene
    };

    class Entity;
    class EntityComponent;

    // Handles external & internal resources
    class ResourceManager
    {
    public:
        ResourceManager() = delete;
        ResourceManager(const string& Path);

        static shared_ptr<SDL_Texture> CreateTexture(SDL_Surface* Surface);
        void CreateEntity(const string& TemplateName, shared_ptr<Entity>& NewEntity);
        
        void LoadResources();
        const nlohmann::json& GetJsonConfig(const string& Name, enum ResourceType Type);
        void RegisterComponent(const string& Type, EntityComponent* Component);
        const EntityComponent* GetComponentPrototypeByName(const string& Name);
        shared_ptr<Entity> CreateEntityFromDataTemplate(const string& Name);
        
        std::shared_ptr<SDL_Texture> GetOrLoadTexture(const string& Path);
        void ReleaseTexture(const string& Path);
        int CountScenes(const string& SceneName) const;

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
