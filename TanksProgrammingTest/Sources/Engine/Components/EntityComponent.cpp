#include "EntityComponent.h"

namespace EngineCore
{
    EntityComponent::EntityComponent(Entity* Owner)
        : m_Owner(Owner)
    {
    }

    void EntityComponent::LoadFromConfig(nlohmann::json Config)
    {
    }

    void EntityComponent::Initialize(GameModeBase* Game)
    {
        m_Game = Game;
    }

    void EntityComponent::Update(float DeltaTime)
    {
    }

    void EntityComponent::Draw()
    {
    }

    void EntityComponent::UnInitialize()
    {
        m_Game = nullptr;
    }
}
