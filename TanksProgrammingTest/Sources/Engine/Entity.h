#pragma once

#include <vector>
#include <nlohmann/json.hpp>

namespace Engine
{
    using namespace std;

    class EntityComponent;

    class Entity
    {
    public:
        void LoadFromConfig(nlohmann::json Config);
        void Initialize();
        void Update(float DeltaTime);
        void Draw();
        void UnInitialize();
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
        list<shared_ptr<EntityComponent>> m_Components;
        string m_Name;
        bool bPendingDestroy = false;
    };
}
