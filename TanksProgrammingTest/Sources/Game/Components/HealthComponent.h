#pragma once
#include "Engine.h"
#include "Event.h"
#include "Components/EntityComponent.h"
#include "Components/TeamComponent.h"

namespace Game
{
    using namespace EngineCore;

    class HealthComponent : public EntityComponent
    {
    public:
        HealthComponent(Entity* Owner);
        HealthComponent();

        void LoadFromConfig(nlohmann::json Config) override;
        void Initialize(GameModeBase* Game) override;
        void Update(float DeltaTime) override;

        bool TryApplyDamage(Entity* Applier = nullptr);
        void Kill() const;
        bool IsInvulnerable() const {return m_IsInvulnerableCounter > 0;}
        void UnInitialize() override;

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
        int GetCurrentHealth() const {return m_CurrentHealth;}

    private:
        const float InvulnerableOnDamageDuration = 1.0f;

        weak_ptr<TeamComponent> m_TeamComponent;
        
        short m_IsInvulnerableCounter = 0;
        float m_InvulnerableTimer = 0.0f;

        short m_StartHealth = 0;
        short m_CurrentHealth = 0;
    };
}
