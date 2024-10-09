#pragma once
#include <map>
#include <memory>
#include "Event.h"
#include "GUI.h"

namespace Game
{
    class HealthComponent;
    class PlayerComponent;
    class TanksGame;

    class TanksGUI : public EngineCore::GUI
    {
    public:
        TanksGUI(const std::shared_ptr<TanksGame>& Game);
        void OnStageChanged() const;

        void OnPlayerHealthChanged(const std::shared_ptr<EngineCore::Entity>& Player, const HealthComponent& Health);
        void OnGameStarted();
        void Update(float DeltaTime) override;
        void UnInitialize() override;

    private :
        std::shared_ptr<EngineCore::TextWidget> m_StageVictoryLabelWidget;
        std::shared_ptr<EngineCore::TextWidget> m_StageVictoryTimerWidget;
        std::map<EngineCore::Entity*, std::shared_ptr<EngineCore::TextWidget>> m_PlayerHealthWidgets;
        std::shared_ptr<TanksGame> m_Game;
        float m_TimerUpdateCounter = 1.0f;

        EngineCore::ListenerHandle m_GameStartedHandle;
        EngineCore::ListenerHandle m_StageChangedHandle;
        std::map<EngineCore::Entity*, EngineCore::ListenerHandle> m_PlayerHealthChangedHandlers;
    };
}
