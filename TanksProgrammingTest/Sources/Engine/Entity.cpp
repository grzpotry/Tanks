#include "Entity.h"
#include "Components/EntityComponent.h"
#include "Engine.h"
#include "ResourceManager.h"

namespace EngineCore
{
    void Entity::LoadFromConfig(nlohmann::json Config)
    {
        m_Name = Config.value("Name", "");

        const auto ResourceManager = Engine::Get()->GetResourceManager();

        if (Config.find("Components") != Config.end())
        {
            for (auto ComponentItem : Config["Components"].items())
            {
                nlohmann::json ComponentConfig = ComponentItem.value();
                string Type = ComponentConfig["Type"];
                const EntityComponent* ComponentPrototype = ResourceManager->GetComponentPrototypeByName(Type);
                unique_ptr<EntityComponent> NewComponent = ComponentPrototype->Clone();

                NewComponent->SetOwner(this);
                NewComponent->LoadFromConfig(ComponentConfig);
                AddComponent(std::move(NewComponent));
            }
        }
    }

    void Entity::Initialize(GameModeBase* const Game) const
    {
        for (const auto& Component : m_Components)
        {
            Component->Initialize(Game);
        }
    }

    void Entity::Update(float DeltaTime)
    {
        for (const auto& Component : m_Components)
        {
            Component->Update(DeltaTime);
        }
    }

    void Entity::Draw()
    {
        for (const auto& Component : m_Components)
        {
            Component->Draw();
        }
    }

    void Entity::UnInitialize()
    {
        for (const auto& Component : m_Components)
        {
            Component->UnInitialize();
        }
    }

    void Entity::Destroy()
    {
        UnInitialize();
        m_Components.clear();
    }

    void Entity::MarkDestroy()
    {
        bPendingDestroy = true;
    }

    void Entity::AddComponent(unique_ptr<EntityComponent> Component)
    {
        m_Components.push_back(std::move(Component));
    }

    void Entity::RemoveComponent(EntityComponent* Component)
    {
        //m_Components.remove(Component);
    }
}
