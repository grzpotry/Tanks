#include "TanksGUI.h"
#include "EngineUtils.h"
#include "TanksGame.h"
#include "Components/HealthComponent.h"

Game::TanksGUI::TanksGUI(const shared_ptr<TanksGame>& Game)
{
    checkMsg(Game.get(), "null game");
    
    m_Game = Game;
    
    m_GameStartedHandle = m_Game->GameStarted->Subscribe([this]()
    {
        OnGameStarted();
    });

    m_StageChangedHandle= m_Game->StageChanged->Subscribe([this]()
    {
        OnStageChanged();
    });

    m_StageTransitionStartedHandle = m_Game->StageTransitionStarted->Subscribe([this]()
    {
        OnStageTransitionStarted();
    });

    m_StageTransitionFinishedHandle = m_Game->StageTransitionFinished->Subscribe([this]()
    {
        OnStageTransitionFinished();
    });

    m_GameOverTriggeredHandle = m_Game->GameOverTriggered->Subscribe([this](const string& Reason)
     {
         OnGameOverTriggered(Reason);
     });

    m_GameWonTriggeredHandle = m_Game->GameWonTriggered->Subscribe([this]()
      {
          OnGameWonTriggered();
      });
}

void Game::TanksGUI::OnGameOverTriggered(const string& Reason)
{
    const auto Rect1 = GetLayoutRect(MiddleCenter, 200, 100);
    m_GameOverLabelWidget = CreateTextWidget("GameOver", Rect1, Medium);

    auto Rect2 = GetLayoutRect(MiddleCenter, 100, 50);
    Rect2.y += 50;
    m_GameOverDescriptionWidget = CreateTextWidget(Reason, Rect2, Small);
}

void Game::TanksGUI::OnGameWonTriggered()
{
    m_GameWonLabelWidget = CreateTextWidget("Kim jestes ? Jestes zwyciezca", GetLayoutRect(MiddleCenter, 500, 100), Medium);
}

void Game::TanksGUI::OnStageTransitionStarted()
{
    const string Text = "Stage " + std::to_string(m_Game->GetCurrentStage()); 
    m_StageTransitionWidget = CreateTextWidget(Text, GetLayoutRect(MiddleCenter, 200, 100), Medium);
}

void Game::TanksGUI::OnStageTransitionFinished()
{
    RemoveWidget(m_StageTransitionWidget);
    m_StageTransitionWidget = nullptr;
}

void Game::TanksGUI::OnStageChanged() const
{
    const string Text = "Stage " + std::to_string(m_Game->GetCurrentStage());
    m_StageVictoryLabelWidget->Update(Text);
}

void Game::TanksGUI::OnPlayerHealthChanged(const shared_ptr<Entity>& Player, const HealthComponent& Health)
{
    const string text = std::to_string(Health.GetCurrentHealth());
    m_PlayerHealthWidgets[Player.get()]->Update(text);
}

void Game::TanksGUI::OnGameStarted()
{
    for (auto& Player : m_Game->GetPlayers())
    {
        const auto Name = Player->GetName();
        if (const auto PlayerHealth = Player->GetComponentWeak<HealthComponent>().lock())
        {
            auto PlayerRect = GetLayoutRect(BottomLeft, 100, 40);
            m_PlayerLabelWidgets.emplace(Player.get(), CreateTextWidget("Lives", PlayerRect));

            PlayerRect.y+= 30;
            PlayerRect.x += 35;
            PlayerRect.w = 30;
            m_PlayerHealthWidgets.emplace(Player.get(), CreateTextWidget(std::to_string(PlayerHealth->GetCurrentHealth()), PlayerRect));

            m_PlayerHealthChangedHandlers.emplace(Player.get(), PlayerHealth->OnHealthChanged->Subscribe([this, &Player](const HealthComponent& Health)
            {
                OnPlayerHealthChanged(Player, Health);
            }));
        }
    }

    const auto StageLabelRect = GetLayoutRect(BottomCenter, 100, 40);
    auto StageTimerRect = StageLabelRect;
    
    StageTimerRect.x += 35;
    StageTimerRect.y += 30;
    StageTimerRect.w = 30;

    m_StageVictoryLabelWidget = CreateTextWidget("Next stage", StageLabelRect);
    m_StageVictoryTimerWidget = CreateTextWidget("Stage timer", StageTimerRect);
}

void Game::TanksGUI::Update(float DeltaTime)
{
    if (!m_Game->IsGameRunning())
    {
        return;
    }
    
    const auto ActiveScene = m_Game->GetActiveScene();

    m_StageVictoryLabelWidget->SetIsVisible(ActiveScene != nullptr);
    m_StageVictoryTimerWidget->SetIsVisible(ActiveScene != nullptr);

    for (const auto& Player : m_Game->GetPlayers())
    {
        if (Player)
        {
            m_PlayerHealthWidgets[Player.get()]->SetIsVisible(ActiveScene != nullptr);
            m_PlayerLabelWidgets[Player.get()]->SetIsVisible(ActiveScene != nullptr);
        }
    }
    
    if (!ActiveScene)
    {
        return;
    }
    
    m_TimerUpdateCounter += DeltaTime;

    if (m_TimerUpdateCounter > 1)
    {
        m_TimerUpdateCounter = 0.0f;
        string text = std::to_string(static_cast<int>(ActiveScene->GetTimeToVictory()));
        m_StageVictoryTimerWidget->Update(text);
    }
}

void Game::TanksGUI::UnInitialize()
{
    m_Game->GameStarted->Unsubscribe(m_GameStartedHandle);

    for (const auto PlayerHandler : m_PlayerHealthChangedHandlers)
    {
        if (const auto PlayerHealth = PlayerHandler.first->GetComponentWeak<HealthComponent>().lock())
        {
            if (PlayerHealth->OnHealthChanged)
            {
                PlayerHealth->OnHealthChanged->Unsubscribe(PlayerHandler.second);
            }
        }
    }

    m_GameStartedHandle = -1;
    m_PlayerHealthChangedHandlers.clear();

    m_Game = nullptr;
}
