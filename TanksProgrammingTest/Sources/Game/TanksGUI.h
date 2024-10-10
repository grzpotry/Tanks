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

    // User interface for Tanks game
    class TanksGUI : public EngineCore::GUI
    {
    public:
        TanksGUI(const std::shared_ptr<TanksGame>& Game);

        void OnGameStarted();
        void Update(float DeltaTime) override;
        void UnInitialize() override;
        void OnGameOverTriggered(const std::string& Reason);
        void OnGameWonTriggered();
        void OnStageTransitionStarted();
        void OnStageTransitionFinished();
        void OnStageChanged() const;
        void OnPlayerHealthChanged(const std::shared_ptr<EngineCore::Entity>& Player, const HealthComponent& Health);

    private:
        float m_TimerUpdateCounter = 1.0f;

        std::shared_ptr<TanksGame> m_Game;
        
        std::shared_ptr<EngineCore::TextWidget> m_GameWonLabelWidget;
        std::shared_ptr<EngineCore::TextWidget> m_GameOverLabelWidget;
        std::shared_ptr<EngineCore::TextWidget> m_GameOverDescriptionWidget;
        
        std::shared_ptr<EngineCore::TextWidget> m_StageVictoryLabelWidget;
        std::shared_ptr<EngineCore::TextWidget> m_StageTransitionWidget;
        std::shared_ptr<EngineCore::TextWidget> m_StageVictoryTimerWidget;
        
        std::map<EngineCore::Entity*, std::shared_ptr<EngineCore::TextWidget>> m_PlayerHealthWidgets;
        std::map<EngineCore::Entity*, std::shared_ptr<EngineCore::TextWidget>> m_PlayerLabelWidgets;

        EngineCore::ListenerHandle m_GameStartedHandle;
        EngineCore::ListenerHandle m_StageChangedHandle;
        EngineCore::ListenerHandle m_StageTransitionStartedHandle;
        EngineCore::ListenerHandle m_StageTransitionFinishedHandle;
        EngineCore::ListenerHandle m_GameOverTriggeredHandle;
        EngineCore::ListenerHandle m_GameWonTriggeredHandle;
        
        std::map<EngineCore::Entity*, EngineCore::ListenerHandle> m_PlayerHealthChangedHandlers;
    };
}
