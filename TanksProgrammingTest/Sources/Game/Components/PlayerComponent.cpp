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

    void PlayerComponent::UnInitialize()
    {
        EntityComponent::UnInitialize();
    }

    void PlayerComponent::Update(float DeltaTime)
    {
        EntityComponent::Update(DeltaTime);

        Timer += DeltaTime;

        if (Timer > 1)
        {
            Timer = 0;

            // debug self damage notifications
            // if (const auto PlayerHealth = m_PlayerHealth.lock())
            // {
            //     PlayerHealth->ApplyDamage();
            // }
        }
    }

}
