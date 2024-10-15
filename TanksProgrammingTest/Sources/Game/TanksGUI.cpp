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

    m_StageChangedHandle = m_Game->StageChanged->Subscribe([this]()
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

    m_GameMenuTriggeredHandle = m_Game->MainMenuTriggered->Subscribe([this]()
    {
        OnMainMenuTriggered();
    });
}

void Game::TanksGUI::OnGameOverTriggered(const string& Reason)
{
    ClearGameWidgets();
    UnsubscribePlayers();
    
    const auto Rect1 = GetLayoutRect(MiddleCenter, 200, 100);
    CreateTextWidget("GameOver", Rect1, &m_GameFinishedWidgets, Medium);

    auto Rect2 = GetLayoutRect(MiddleCenter, 150, 50);
    Rect2.y += 50;
    CreateTextWidget(Reason, Rect2, &m_GameFinishedWidgets,Small);
}

void Game::TanksGUI::OnGameWonTriggered()
{
    ClearGameWidgets();
    UnsubscribePlayers();
    
    CreateTextWidget("Kim jestes ? Jestes zwyciezca", GetLayoutRect(MiddleCenter, 500, 100), &m_GameFinishedWidgets, Medium);
}

void Game::TanksGUI::ClearGameWidgets()
{
    if (m_StageVictoryLabelWidget)
    {
        RemoveWidget(m_StageVictoryLabelWidget);
        m_StageVictoryLabelWidget = nullptr;
    }

    if (m_StageVictoryTimerWidget)
    {
        RemoveWidget(m_StageVictoryTimerWidget);
        m_StageVictoryTimerWidget = nullptr;
    }

    if (m_StageTransitionWidget)
    {
        RemoveWidget(m_StageTransitionWidget);
        m_StageTransitionWidget = nullptr;
    }

    for (const auto& PlayerHealthWidget : m_PlayerHealthWidgets)
    {
        RemoveWidget(PlayerHealthWidget.second);
    }

    m_PlayerHealthWidgets.clear();

    for (const auto& PlayerLabelWidget : m_PlayerLabelWidgets)
    {
        RemoveWidget(PlayerLabelWidget.second);
    }
    
    m_PlayerLabelWidgets.clear();

    ClearWidgets(m_GameFinishedWidgets);
}

void Game::TanksGUI::ClearMainMenu()
{
    m_IsMainMenuActive = false;
    ClearWidgets(m_MainMenuWidgets);
}

void Game::TanksGUI::OnMainMenuTriggered()
{
    m_IsMainMenuActive = true;
    printf("MainMenu \n");
    
    ClearGameWidgets();
    
    auto TitleRect = GetLayoutRect(MiddleCenter, 500, 100);
    TitleRect.y -= 50;
    CreateTextWidget("TANKS", TitleRect, &m_MainMenuWidgets, Large, {255, 0, 0, 255});

    auto Rect3 = GetLayoutRect(MiddleCenter, 400, 50);
    Rect3.y += 50;
    m_StartGameWidget = CreateTextWidget("Press 1 to Start Game", Rect3, &m_MainMenuWidgets, Small, {255, 255, 0, 255});

    auto Rect4 = GetLayoutRect(MiddleCenter, 350, 50);
    Rect4.y += 100;
    CreateTextWidget("Press ESC to Exit", Rect4, &m_MainMenuWidgets, Small);

    auto Rect2 = GetLayoutRect(MiddleCenter, 350, 50);
    Rect2.y += 200;
    CreateTextWidget("Movement [WSAD / ARROWS]", Rect2, &m_MainMenuWidgets, Small);

    auto Rect5 = GetLayoutRect(MiddleCenter, 300, 50);
    Rect5.y += 250;
   CreateTextWidget("Shoot [SPACE]", Rect5, &m_MainMenuWidgets, Small);
}

void Game::TanksGUI::OnStageTransitionStarted()
{
    const string Text = "Stage " + std::to_string(m_Game->GetCurrentStage()); 
    m_StageTransitionWidget = CreateTextWidget(Text, GetLayoutRect(MiddleCenter, 200, 100), nullptr, Medium);
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

void Game::TanksGUI::ClearWidgets(std::vector<shared_ptr<TextWidget>>& Widgets)
{
    for (const auto& text_widget : Widgets)
    {
        RemoveWidget(text_widget);
    }

    Widgets.clear();
}

void Game::TanksGUI::OnGameStarted()
{
    ClearMainMenu();
    
    for (auto& Player : m_Game->GetPlayers())
    {
        const auto Name = Player->GetName();
        if (const auto PlayerHealth = Player->GetComponentWeak<HealthComponent>().lock())
        {
            auto PlayerRect = GetLayoutRect(BottomLeft, 100, 40);
            m_PlayerLabelWidgets.emplace(Player.get(), CreateTextWidget("Lives", PlayerRect, nullptr));

            PlayerRect.y+= 30;
            PlayerRect.x += 35;
            PlayerRect.w = 30;
            m_PlayerHealthWidgets.emplace(Player.get(), CreateTextWidget(std::to_string(PlayerHealth->GetCurrentHealth()), PlayerRect, nullptr));

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

    m_StageVictoryLabelWidget = CreateTextWidget("Next stage", StageLabelRect, nullptr);
    m_StageVictoryTimerWidget = CreateTextWidget("Stage timer", StageTimerRect, nullptr);
}

void Game::TanksGUI::HandleMainMenuInput() const
{
    for (const SDL_Event& Event : Engine::Get()->GetEvents())
    {
        switch (Event.type)
        {
            case SDL_KEYDOWN:
                {
                    switch (Event.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_1:
                        {
                            m_Game->StartNewGame();
                        }
                        break;
                    default:
                        break;
                    }
                }
        }
    }
}

void Game::TanksGUI::Update(float DeltaTime)
{
    if (m_IsMainMenuActive && !m_Game->IsGameRunning())
    {
        m_StartGameBlinkingTimer += DeltaTime;
        
        m_StartGameWidget->SetIsVisible(m_StartGameBlinkingTimer < StartGameBlinkTime);
        
        if (m_StartGameBlinkingTimer > StartGameBlinkTime * 2)
        {
            m_StartGameBlinkingTimer = 0.0f;
        }
        
        HandleMainMenuInput();
        return;
    }

    m_StartGameBlinkingTimer = 0.0f;
    const auto ActiveScene = m_Game->GetActiveScene();

    if (m_StageVictoryLabelWidget)
    {
        m_StageVictoryLabelWidget->SetIsVisible(ActiveScene != nullptr);
    }

    if (m_StageVictoryTimerWidget)
    {
        m_StageVictoryTimerWidget->SetIsVisible(ActiveScene != nullptr);
    }

    for (const auto& Player : m_Game->GetPlayers())
    {
        auto it = m_PlayerHealthWidgets.find(Player.get());
        if (it != m_PlayerHealthWidgets.end())
        {
            it->second->SetIsVisible(ActiveScene != nullptr);
        }
        auto it2 = m_PlayerLabelWidgets.find(Player.get());
        if (it2 != m_PlayerLabelWidgets.end())
        {
            it2->second->SetIsVisible(ActiveScene != nullptr);
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

void Game::TanksGUI::UnsubscribePlayers()
{
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

    m_PlayerHealthChangedHandlers.clear();
}

void Game::TanksGUI::UnInitialize()
{
    m_Game->GameStarted->Unsubscribe(m_GameStartedHandle);
    m_Game->StageChanged->Unsubscribe(m_StageChangedHandle);
    m_Game->StageTransitionStarted->Unsubscribe(m_StageTransitionStartedHandle);
    m_Game->StageTransitionFinished->Unsubscribe(m_StageTransitionFinishedHandle);
    m_Game->GameOverTriggered->Unsubscribe(m_GameOverTriggeredHandle);
    m_Game->GameWonTriggered->Unsubscribe(m_GameWonTriggeredHandle);
    m_Game->MainMenuTriggered->Unsubscribe(m_GameMenuTriggeredHandle);
    
    UnsubscribePlayers();
    m_GameStartedHandle = -1;
    m_Game = nullptr;
}
