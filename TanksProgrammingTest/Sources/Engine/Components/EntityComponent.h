#pragma once
#include <nlohmann/json.hpp>

namespace EngineCore
{
    class GameModeBase;
    class Scene;
    class PhysicsComponent;
    class Entity;
    
    using namespace std;

    /*
     */
    class EntityComponent
    {
    public:
        virtual ~EntityComponent()
        {
            //printf("Destroy component");
        }

        EntityComponent() = delete;
        EntityComponent(Entity* Owner);

        virtual unique_ptr<EntityComponent> Clone() const = 0;

        virtual void LoadFromConfig(nlohmann::json Config);
        virtual void Initialize(GameModeBase* Game);
        virtual void Update(float DeltaTime);
        virtual void Draw();
        virtual void UnInitialize();

        constexpr bool IsValid() const { return m_Owner != nullptr; }
        constexpr Entity* GetOwner() const { return m_Owner; }
        void SetOwner(Entity* Owner) { m_Owner = Owner; }

    protected:
        //~EntityComponent() = default;
        GameModeBase* m_Game = nullptr;

    private:
        Entity* m_Owner;
    };
}
