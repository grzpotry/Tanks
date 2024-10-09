#include "TeamComponent.h"

namespace EngineCore
{
    TeamComponent::TeamComponent(Entity* Owner)
        : EntityComponent(Owner)
    {
    }

    TeamComponent::TeamComponent() : TeamComponent(nullptr)
    {
    }

    void TeamComponent::LoadFromConfig(nlohmann::json Config)
    {
        m_TeamId = Config.value("TeamId", 0);
    }

    void TeamComponent::SetTeamId(int TeamId)
    {
        m_TeamId = TeamId;
    }
}
