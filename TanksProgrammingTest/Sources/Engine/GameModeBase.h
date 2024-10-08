#pragma once

#include "Scene.h"

namespace EngineCore
{
    class Scene;
    class ResourceManager;
    class Engine;

    class GameModeBase
    {
    public:
        virtual ~GameModeBase() = default;

        GameModeBase(Engine* Engine, ResourceManager* ResourcesManager): m_Engine(Engine),
                                                                         m_ResourceManager(ResourcesManager),
                                                                         m_ActiveScene(nullptr)
        {
        }

        virtual void Start() = 0;

        virtual void Initialize();
        virtual void UnInitialize();
        virtual void Update(float DeltaTime);
        virtual void Draw() const;

        Scene* GetActiveScene() const { return m_ActiveScene; };

    protected:
        Engine* m_Engine;
        ResourceManager* m_ResourceManager;
        Scene* m_ActiveScene;
    };
}
