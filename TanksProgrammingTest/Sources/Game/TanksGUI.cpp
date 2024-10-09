#include "TanksGUI.h"
#include "EngineUtils.h"
#include "TanksGame.h"
#include "Components/HealthComponent.h"
#include "Components/PlayerComponent.h"

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
}

void Game::TanksGUI::OnStageChanged() const
{
    string text = "Stage: " + std::to_string(m_Game->GetCurrentStage());
    m_StageVictoryLabelWidget->Update(text);
}

void Game::TanksGUI::OnPlayerHealthChanged(const shared_ptr<Entity>& Player, const HealthComponent& Health)
{
    string text = Player->GetName() + " : " + std::to_string(Health.GetCurrentHealth());
    m_PlayerHealthWidgets[Player.get()]->Update(text);
    printf("player health changed\n");
}

void Game::TanksGUI::OnGameStarted()
{
    printf("Ongamestarted \n");
    for (auto& Player : m_Game->GetPlayers())
    {
        auto Name = Player->GetName();
        //TODO: support for more widgets & players
        SDL_Rect Rect;
        Rect.x = 10;
        Rect.y = 570;
        Rect.w = 100;
        Rect.h = 40;
        if (auto c = Player->GetComponentWeak<TeamComponent>().lock())
        {
            printf("");
        }

        if (const auto PlayerHealth = Player->GetComponentWeak<HealthComponent>().lock())
        {
            string text = Name + " : " + std::to_string(PlayerHealth->GetCurrentHealth());
            m_PlayerHealthWidgets.emplace(Player.get(), CreateTextWidget(text, Rect));

            m_PlayerHealthChangedHandlers.emplace(Player.get(), PlayerHealth->OnHealthChanged->Subscribe([this, &Player](const HealthComponent& Health)
            {
                OnPlayerHealthChanged(Player, Health);
            }));
        }

        SDL_Rect Rect2;
        Rect2.x = 400;
        Rect2.y = 570;
        Rect2.w = 100;
        Rect2.h = 30;
        m_StageVictoryLabelWidget = CreateTextWidget("Next stage", Rect2);

        Rect2.y = 600;
        Rect2.w = 30;
        Rect2.h = 30;
        Rect2.x = 430;
        m_StageVictoryTimerWidget = CreateTextWidget("Stage timer", Rect2);
    }
}

void Game::TanksGUI::Update(float DeltaTime)
{
    m_TimerUpdateCounter += DeltaTime;

    if (m_TimerUpdateCounter > 1)
    {
        m_TimerUpdateCounter = 0.0f;
        string text = std::to_string(static_cast<int>(m_Game->GetActiveScene()->GetTimeToVictory()));
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
            PlayerHealth->OnHealthChanged->Unsubscribe(PlayerHandler.second);
        }
    }

    m_GameStartedHandle = -1;
    m_PlayerHealthChangedHandlers.clear();

    m_Game = nullptr;
}
