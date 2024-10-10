#include "PlayerComponent.h"
#include "Entity.h"
#include "HealthComponent.h"

namespace Game
{
    PlayerComponent::PlayerComponent(Entity* Owner)
       : EntityComponent(Owner)
    {
    }

    PlayerComponent::PlayerComponent() : PlayerComponent(nullptr)
    {
    }

    void PlayerComponent::LoadFromConfig(nlohmann::json Config)
    {
        m_Name = Config.value("Name", "default_player");
    }

    void PlayerComponent::Initialize(GameModeBase* Game)
    {
        EntityComponent::Initialize(Game);
        m_PlayerHealth = GetOwner()->GetComponentWeak<HealthComponent>();
    }
}
