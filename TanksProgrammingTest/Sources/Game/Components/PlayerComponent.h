#pragma once
#include "Components/EntityComponent.h"

namespace Game
{
    class HealthComponent;

    // Deprecated - can be removed
    class PlayerComponent : public EngineCore::EntityComponent
    {
    public:
        PlayerComponent(EngineCore::Entity* Owner);
        PlayerComponent();

        [[nodiscard]] std::unique_ptr<EntityComponent> Clone() const override
        {
            return std::make_unique<PlayerComponent>(*this);
        }

        void LoadFromConfig(nlohmann::json Config) override;
        void Initialize(EngineCore::GameModeBase* Game) override;

        std::string& GetName() { return m_Name; }

    private:
        std::string m_Name;
        std::weak_ptr<HealthComponent> m_PlayerHealth;
    };
}
