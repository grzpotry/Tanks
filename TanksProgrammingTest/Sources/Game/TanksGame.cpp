#include "TanksGame.h"

#include "EngineUtils.h"
#include "Components/HealthComponent.h"

namespace Game
{
    void TanksGame::CreateNewScene(nlohmann::json Config)
    {
        if (m_ActiveScene != nullptr)
        {
            m_ActiveScene = nullptr;
        }
        
        const auto basic_string = Config.value("Name", "");
        const float TimeToVictory = Config.value("TimeToVictory", 0.0f);
        const float EnemyRespawnCooldown = Config.value("EnemyRespawnCooldown", 1.0f);
        const float MinEnemies = Config.value("MinEnemies", 1.0f);
        
        m_ActiveScene = make_shared<Scene>(basic_string, TimeToVictory, EnemyRespawnCooldown, MinEnemies);

        printf("LoadSceneFromConfig \n");

        if (const nlohmann::json::const_iterator SceneLayoutIt = Config.find("SceneLayout"); SceneLayoutIt != Config.
            end())
        {
            m_ActiveScene->LoadSceneFromLayout((*SceneLayoutIt)["Content"], (*SceneLayoutIt)["Legend"],
                                               (*SceneLayoutIt)["PositionMarkers"]);
        }

        m_ActiveScene->Initialize(this);
    }

    string TanksGame::GetCurrentSceneName(unsigned StageIndex) const
    {
        return "Level" + std::to_string(StageIndex);
    }

    bool TanksGame::LoadNextScene()
    {
        const string SceneName = GetCurrentSceneName(m_CurrentStage);
        const nlohmann::json SceneConfig = m_ResourceManager->GetJsonConfig(SceneName, ResourceType::Scene);

        if (SceneConfig.is_null())
        {
            return false;
        }
        
        CreateNewScene(SceneConfig);
        return true;
    }

    bool TanksGame::IsSceneAvailable(const string& SceneName) const
    {
        return m_ResourceManager->CountScenes(SceneName) > 0;
    }

    void TanksGame::Start()
    {
        checkMsg(m_ResourceManager != nullptr, "Can't initiate resources");

        shared_ptr<Entity> Player;
   
        m_ResourceManager->CreateEntity("Player", Player);
        m_Players.push_back(Player);

        m_IsGameRunning = true;

        if (GameStarted)
        {
            GameStarted->Invoke();
        }
    }

    void TanksGame::StartStageTransition()
    {
        m_CurrentStage++;

        auto SceneName = GetCurrentSceneName(m_CurrentStage);
        
        if (!IsSceneAvailable(SceneName))
        {
            RequestGameWon();
            return;
        }
        
        m_AnyStageLoaded = true;
        m_IsTransitioningToNextStage = true;
        m_NextStageTimer = TimeBetweenStages;

        if (StageTransitionStarted)
        {
            StageTransitionStarted->Invoke();
        }
    }

    void TanksGame::FinishStageTransition()
    {
        m_IsTransitioningToNextStage = false;

        if (StageTransitionFinished)
        {
            StageTransitionFinished->Invoke();
        }
                
        if (LoadNextScene())
        {
            for (const auto& Player : m_Players)
            {
                m_ActiveScene->AddPlayer(Player);
            }

            if (StageChanged)
            {
                StageChanged->Invoke();
            }
        }
    }

    void TanksGame::Update(float DeltaTime)
    {
        if (m_IsGameFinishRequested && m_ActiveScene)
        {
            DestroyActiveScene();
            return;
        }
        
        GameModeBase::Update(DeltaTime);
        
        if (!m_IsGameRunning)
        {
            return;
        }
        
        if (!m_AnyStageLoaded || m_ActiveScene && m_ActiveScene->GetTimeToVictory() <= 0)
        {
            DestroyActiveScene();
            StartStageTransition();
        }

        if (m_IsTransitioningToNextStage)
        {
            m_NextStageTimer -= DeltaTime;

            if (m_NextStageTimer <= 0)
            {
                FinishStageTransition();
            }
        }
    }

    void TanksGame::DestroyActiveScene()
    {
        if (m_ActiveScene)
        {
            // remove player from scene first, so it won't be reinitialized when spawned on next scene
            for (const auto& Player : m_Players)
            {
                m_ActiveScene->RemoveEntity(Player);
            }
            m_ActiveScene->UnInitialize();
        }
        m_ActiveScene = nullptr;
    }

    void TanksGame::RequestGameOver(Entity* SourceEntity)
    {
        m_IsGameFinishRequested = true;
        
        if (GameOverTriggered)
        {
            const string Reason = SourceEntity->GetName() + " is lost";
            GameOverTriggered->Invoke(Reason);
        }
    }

    void TanksGame::RequestGameWon()
    {
        m_IsGameFinishRequested = true;

        if (GameWonTriggered)
        {
            GameWonTriggered->Invoke();
        }
    }

    void TanksGame::UnInitialize()
    {
        printf("UnInitialize TankGame \n");
        GameModeBase::UnInitialize();
    }
}
