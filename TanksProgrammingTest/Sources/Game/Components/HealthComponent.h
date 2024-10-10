#pragma once
#include "Engine.h"
#include "Event.h"
#include "Components/EntityComponent.h"
#include "Components/TeamComponent.h"

namespace Game
{
    using namespace EngineCore;

    // Defines health attribute, such entity can be killed
    class HealthComponent : public EntityComponent
    {
    public:
        HealthComponent(Entity* Owner);
        HealthComponent();
        
        HealthComponent(const HealthComponent& other)
            : EntityComponent(other),
              InvulnerableOnDamageDuration(other.InvulnerableOnDamageDuration),
              m_IsInvulnerableCounter(other.m_IsInvulnerableCounter),
              m_InvulnerableTimer(other.m_InvulnerableTimer),
              m_StartHealth(other.m_StartHealth),
              m_CurrentHealth(other.m_CurrentHealth)
        {
            
            if (other.OnHealthChanged)
            {
                OnHealthChanged = other.OnHealthChanged->Copy();
            }
            else
            {
                OnHealthChanged = nullptr;
            }
        }

        [[nodiscard]] std::unique_ptr<EntityComponent> Clone() const override
        {
            return std::make_unique<HealthComponent>(*this);
        }

        shared_ptr<Event<HealthComponent&>> OnHealthChanged;
        
        void LoadFromConfig(nlohmann::json Config) override;
        void Initialize(GameModeBase* Game) override;
        void UnInitialize() override;
        void Update(float DeltaTime) override;
        bool TryApplyDamage(Entity* Applier = nullptr);
        void Kill() const;
        
        int GetCurrentHealth() const { return m_CurrentHealth; }
        bool IsInvulnerable() const { return m_IsInvulnerableCounter > 0; }

    private:
        const float InvulnerableOnDamageDuration = 0.5f;
        bool m_TriggerGameOverOnKill = false;
        short m_IsInvulnerableCounter = 0;
        float m_InvulnerableTimer = 0.0f;
        int m_StartHealth = 0;
        int m_CurrentHealth = 0;

        weak_ptr<TeamComponent> m_TeamComponent;
    };
}
