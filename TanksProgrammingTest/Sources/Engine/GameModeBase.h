#pragma once

#include "Scene.h"

namespace EngineCore
{
    class Scene;
    class ResourceManager;
    class Engine;

    // Basic game mode, handles game rules
    class GameModeBase
    {
    public:
        virtual ~GameModeBase() = default;

        GameModeBase(Engine* Engine, const shared_ptr<ResourceManager>& ResourcesManager): m_Engine(Engine),
                                                                                           m_ResourceManager(ResourcesManager),
                                                                                           m_ActiveScene(nullptr)
        {
        }

        virtual void Start() = 0;
        virtual void Initialize();
        virtual void UnInitialize();
        virtual void Update(float DeltaTime);
        virtual void Draw() const;
        virtual void RequestGameOver(Entity* SourceEntity) = 0;

       std::shared_ptr<Scene> GetActiveScene() const { return m_ActiveScene; }

    protected:
        Engine* m_Engine;
        shared_ptr<ResourceManager> m_ResourceManager;
        std::shared_ptr<Scene> m_ActiveScene;
    };
}
