#include "HealthComponent.h"

namespace Game
{
    HealthComponent::HealthComponent(Entity* Owner)
        : EntityComponent(Owner)
    {
        OnHealthChanged = make_shared<Event<HealthComponent&>>();
    }

    HealthComponent::HealthComponent() : HealthComponent(nullptr)
    {
        printf("HealthComponent.() \n");
    }

    void HealthComponent::UnInitialize()
    {
        OnHealthChanged = nullptr;
        EntityComponent::UnInitialize();
    }

    void HealthComponent::LoadFromConfig(nlohmann::json Config)
    {
        m_StartHealth = Config.value("StartValue", 1);
    }

    void HealthComponent::Initialize(GameModeBase* Game)
    {
        EntityComponent::Initialize(Game);

        m_CurrentHealth = m_StartHealth;
    }

    void HealthComponent::Update(float DeltaTime)
    {
        EntityComponent::Update(DeltaTime);

        if (IsInvulnerable())
        {
            m_InvulnerableTimer += DeltaTime;
        }

        if (m_InvulnerableTimer > InvulnerableOnDamageDuration)
        {
            m_InvulnerableTimer = 0.0f;
            m_IsInvulnerableCounter--;
        }
    }

    void HealthComponent::ApplyDamage()
    {
        if (IsInvulnerable() || GetOwner()->IsPendingDestroy())
        {
            printf("INULNERABLE \n");
            return;
        }

        m_InvulnerableTimer = 0.0f;
        m_IsInvulnerableCounter++;
        m_CurrentHealth --;

        if (OnHealthChanged)
        {
            OnHealthChanged->Invoke(*this);
        }
        
        printf("APPLIED DAMAGE \n");

        if (m_CurrentHealth <= 0)
        {
            Kill();
        }
    }

    void HealthComponent::Kill() const
    {
        GetOwner()->MarkDestroy();
    }
}
