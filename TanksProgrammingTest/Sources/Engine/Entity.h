#pragma once

#include <nlohmann/json.hpp>

namespace Engine
{
    using namespace std;

    class EntityComponent;

    class Entity : public std::enable_shared_from_this<Entity>
    {
    public:
        ~Entity()
        {
            // printf("Destroy entity \n");
        }

        void LoadFromConfig(nlohmann::json Config);
        void Initialize();
        void Update(float DeltaTime);
        void Draw();
        void UnInitialize();
        void MarkDestroy();
        void Destroy();

        void AddComponent(unique_ptr<EntityComponent> Component);
        void RemoveComponent(EntityComponent* Component);
        void SetParent(const weak_ptr<Entity>& Parent);
        
        string GetName() { return m_Name; }
        bool IsPendingDestroy() const { return bPendingDestroy; }
        weak_ptr<Entity> GetParent() const { return m_Parent; }
        bool IsChildOf(Entity* const Other) const;

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

        std::weak_ptr<Entity> GetWeakRef()
        {
            return shared_from_this();
        }

    private:
        list<shared_ptr<EntityComponent>> m_Components;
        string m_Name;
        bool bPendingDestroy = false;
        weak_ptr<Entity> m_Parent;
    };
}
