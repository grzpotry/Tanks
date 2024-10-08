#pragma once
#include "Engine.h"
#include "Event.h"
#include "Components/EntityComponent.h"

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

        void ApplyDamage();
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
            printf("HealthComponent copy \n");
            
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
        
        int m_IsInvulnerableCounter;
        float m_InvulnerableTimer = 0.0f;

        int m_StartHealth = 0;
        int m_CurrentHealth = 0;
    };
}
