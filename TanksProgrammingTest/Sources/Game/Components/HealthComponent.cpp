#include "HealthComponent.h"

#include "GameModeBase.h"

namespace Game
{
    HealthComponent::HealthComponent(Entity* Owner)
        : EntityComponent(Owner)
    {
        OnHealthChanged = make_shared<Event<HealthComponent&>>();
    }

    HealthComponent::HealthComponent() : HealthComponent(nullptr)
    {
    }

    void HealthComponent::UnInitialize()
    {
        OnHealthChanged = nullptr;
        EntityComponent::UnInitialize();
    }

    void HealthComponent::LoadFromConfig(nlohmann::json Config)
    {
        m_StartHealth = Config.value("StartValue", 1);
        m_TriggerGameOverOnKill = Config.value("TriggersGameOverOnKill", false);
        m_CurrentHealth = m_StartHealth;
    }

    void HealthComponent::Initialize(GameModeBase* Game)
    {
        EntityComponent::Initialize(Game);

        m_CurrentHealth = m_StartHealth;
        m_TeamComponent = GetOwner()->GetComponentWeak<TeamComponent>();
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

    bool HealthComponent::TryApplyDamage(Entity* Applier)
    {
        if (IsInvulnerable() || GetOwner()->IsPendingDestroy())
        {
            return false;
        }

        if (Applier)
        {
            if (const auto SelfTeam = m_TeamComponent.lock())
            {
                if (const auto OtherTeam = Applier->GetComponentWeak<TeamComponent>().lock())
                {
                    if (OtherTeam->GetTeamId() == SelfTeam->GetTeamId())
                    {
                        return false;
                    }
                }
            }
        }

        m_InvulnerableTimer = 0.0f;
        m_IsInvulnerableCounter++;
        m_CurrentHealth --;

        if (OnHealthChanged)
        {
            OnHealthChanged->Invoke(*this);
        }

        if (m_CurrentHealth <= 0)
        {
            Kill();
        }

        return true;
    }

    void HealthComponent::Kill() const
    {
        GetOwner()->MarkDestroy();

        if (m_TriggerGameOverOnKill)
        {
            m_Game->RequestGameOver(GetOwner());
        }
    }
}
