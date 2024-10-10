#pragma once

#include <nlohmann/json.hpp>

namespace EngineCore
{
    using namespace std;
    
    class GameModeBase;
    class Scene;
    class EntityComponent;

    // Can be loaded from file or created in runtime, assigned components control its behaviour
    class Entity
    {
    public:
        ~Entity()
        {
            printf("Destroy entity \n");
        }

        void LoadFromConfig(nlohmann::json Config);
        void Initialize(GameModeBase* const Game) const;
        void UnInitialize();
        void Update(float DeltaTime);
        void Draw();
        void MarkDestroy();
        void Destroy();
        void AddComponent(unique_ptr<EntityComponent> Component);
        void RemoveComponent(EntityComponent* Component);

        string GetName() { return m_Name; }
        bool IsPendingDestroy() const { return bPendingDestroy; }

        template <typename ComponentType>
        weak_ptr<ComponentType> GetComponentWeak()
        {
            for (auto& Component : m_Components)
            {
                if (auto TypedComponent = dynamic_pointer_cast<ComponentType>(Component))
                {
                    return TypedComponent;
                }
            }
            return weak_ptr<ComponentType>();
        }

    private:
        bool bPendingDestroy = false;
        string m_Name;
        
        list<shared_ptr<EntityComponent>> m_Components;
    };
}
