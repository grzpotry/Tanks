#pragma once
#include "EntityComponent.h"

namespace EngineCore
{
    // entity team, helps identify friendly fire
    class TeamComponent : public EntityComponent
    {
    public:
        TeamComponent(Entity* Owner);
        TeamComponent();

        [[nodiscard]] std::unique_ptr<EntityComponent> Clone() const override
        {
            return std::make_unique<TeamComponent>(*this);
        }

        void LoadFromConfig(nlohmann::json Config) override;

        int GetTeamId() const { return m_TeamId; }
        void SetTeamId(int TeamId);

    private:
        int m_TeamId = 0;
    };
}
