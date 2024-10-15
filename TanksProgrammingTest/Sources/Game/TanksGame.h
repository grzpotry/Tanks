#pragma once
#include <memory>
#include "Event.h"
#include "GameModeBase.h"

namespace Game
{
    class HealthComponent;
    class PlayerComponent;
    using namespace EngineCore;

    // Controls game logic, notifies about important game events
    class TanksGame : public GameModeBase
    {
    public:
        TanksGame(Engine* const Engine, shared_ptr<ResourceManager> const& ResourcesManager)
            : GameModeBase(Engine, ResourcesManager)
        {
            GameStarted = make_shared<Event<>>();
            StageChanged = make_shared<Event<>>();
            StageTransitionStarted = make_shared<Event<>>();
            StageTransitionFinished = make_shared<Event<>>();
            GameOverTriggered = make_shared<Event<string>>();
            GameWonTriggered = make_shared<Event<>>();
            MainMenuTriggered = make_shared<Event<>>();
        }

        TanksGame(const TanksGame& other) = delete;
        
        void CreateNewScene(nlohmann::json Config);
        string GetCurrentSceneName(unsigned StageIndex) const;

        void Start() override;
        void StartNewGame();
        void StartGameMenu();
        bool LoadNextScene();
        bool IsSceneAvailable(const string& SceneName) const;

        void StartStageTransition();
        void FinishStageTransition();
        
        void UnInitialize() override;
        void Update(float DeltaTime) override;
        void DestroyActiveScene();
        void FinishGame();
        
        void RequestGameOver(Entity* SourceEntity) override;
        void RequestGameWon();

        bool IsGameRunning() const { return m_IsGameRunning; }
        std::list<shared_ptr<Entity>>& GetPlayers() {return m_Players;}
        unsigned int GetCurrentStage() const { return m_CurrentStage; }
        
        shared_ptr<Event<>> GameStarted;
        shared_ptr<Event<>> StageChanged;
        shared_ptr<Event<>> StageTransitionStarted;
        shared_ptr<Event<>> StageTransitionFinished;
        shared_ptr<Event<string>> GameOverTriggered;
        shared_ptr<Event<>> GameWonTriggered;
        shared_ptr<Event<>> MainMenuTriggered;
    
    private:
        const float TimeBetweenStages = 2.0f;
        const float RestartGameDuration = 3.0f;

        bool m_IsGameFinishRequested = false;
        bool m_AnyStageLoaded = false;
        bool m_IsGameRunning = false;
        bool m_IsTransitioningToNextStage = false;
        float m_NextStageTimer = 0.0f;
        float m_RestartGameTimer = 0.0f;
        bool m_IsGameRestartPending = false;
        unsigned int m_CurrentStage = 0;
        
        std::list<shared_ptr<Entity>> m_Players;
    };
}
