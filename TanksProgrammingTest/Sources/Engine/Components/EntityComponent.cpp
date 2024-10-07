#include "EntityComponent.h"

namespace Engine
{
    EntityComponent::EntityComponent(Entity* Owner)
        : m_Owner(Owner)
    {
    }

    void EntityComponent::LoadFromConfig(nlohmann::json Config)
    {
    }

    void EntityComponent::Initialize(Scene* const Scene)
    {
        m_Scene = Scene;
    }

    void EntityComponent::Update(float DeltaTime)
    {
    }

    void EntityComponent::Draw()
    {
    }

    void EntityComponent::UnInitialize()
    {
    }
}
