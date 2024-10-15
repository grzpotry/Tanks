#pragma once
#include <map>
#include <memory>
#include "Event.h"
#include "GUI.h"
#include "TanksGame.h"

namespace Game
{
    class HealthComponent;
    class PlayerComponent;
    class TanksGame;

    // User interface for Tanks game
    class TanksGUI : public GUI
    {
    public:
        TanksGUI(const std::shared_ptr<TanksGame>& Game);

        void OnGameStarted();
        void Update(float DeltaTime) override;
        void UnsubscribePlayers();
        void UnInitialize() override;
        void OnGameOverTriggered(const std::string& Reason);
        void OnGameWonTriggered();
        void OnMainMenuTriggered();
        void OnStageTransitionStarted();
        void OnStageTransitionFinished();
        void OnStageChanged() const;
        void OnPlayerHealthChanged(const std::shared_ptr<Entity>& Player, const HealthComponent& Health);
        void HandleMainMenuInput() const;

        void ClearGameWidgets();
        void ClearMainMenu();
        void ClearWidgets(std::vector<shared_ptr<TextWidget>>& Widgets);

    private:
        float StartGameBlinkTime = 0.5f;
        float m_StartGameBlinkingTimer = 0.0f;
        float m_TimerUpdateCounter = 1.0f;
        bool m_IsMainMenuActive = false;

        std::shared_ptr<TanksGame> m_Game;

        std::vector<shared_ptr<TextWidget>> m_MainMenuWidgets;
        std::shared_ptr<TextWidget> m_StartGameWidget;
        std::vector<shared_ptr<TextWidget>> m_GameFinishedWidgets;
        
        std::shared_ptr<TextWidget> m_StageVictoryLabelWidget;
        std::shared_ptr<TextWidget> m_StageVictoryTimerWidget;
        std::shared_ptr<TextWidget> m_StageTransitionWidget;
        
        std::map<Entity*, std::shared_ptr<TextWidget>> m_PlayerHealthWidgets;
        std::map<Entity*, std::shared_ptr<TextWidget>> m_PlayerLabelWidgets;

        ListenerHandle m_GameStartedHandle;
        ListenerHandle m_StageChangedHandle;
        ListenerHandle m_StageTransitionStartedHandle;
        ListenerHandle m_StageTransitionFinishedHandle;
        ListenerHandle m_GameOverTriggeredHandle;
        ListenerHandle m_GameWonTriggeredHandle;
        ListenerHandle m_GameMenuTriggeredHandle;
        
        std::map<Entity*, ListenerHandle> m_PlayerHealthChangedHandlers;
    };
}
