#include "TanksGame.h"

#include "EngineUtils.h"
#include "Components/HealthComponent.h"
#include "Components/PlayerComponent.h"

namespace Game
{
    void TanksGame::CreateDefaultEntities(nlohmann::json Config, std::list<shared_ptr<PlayerComponent>>& Players) const
    {
        if (Config.find("Entities") != Config.end())
        {
            ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();

            for (const auto& Item : Config["Entities"].items())
            {
                auto NewEntity = make_unique<Entity>();

                nlohmann::json EntityConfig = Item.value();
                string TypeName = EntityConfig.value("Type", "");

                if (!TypeName.empty())
                {
                    const nlohmann::json EntityTemplateConfig = ResourceManagerPtr->GetJsonConfig(
                        TypeName, ResourceType::Entity);
                    NewEntity->LoadFromConfig(EntityTemplateConfig);
                }
                else
                {
                    NewEntity->LoadFromConfig(Item.value());
                }

                if (auto p = NewEntity->GetComponentWeak<PlayerComponent>().lock())
                {
                    Players.push_back(p);
                }

                m_ActiveScene->AddEntity(std::move(NewEntity));
            }
        }
    }

    void TanksGame::CreateNewScene(nlohmann::json Config, std::list<shared_ptr<PlayerComponent>>& Players)
    {
        const auto basic_string = Config.value("Name", "");
        const float TimeToVictory = Config.value("TimeToVictory", 0.0f);
        m_ActiveScene = new Scene(std::move(basic_string), TimeToVictory);
        
        printf("LoadSceneFromConfig \n");

        CreateDefaultEntities(Config, Players);

        if (const nlohmann::json::const_iterator SceneLayoutIt = Config.find("SceneLayout"); SceneLayoutIt != Config.end())
        {
            m_ActiveScene->LoadSceneFromLayout((*SceneLayoutIt)["Content"], (*SceneLayoutIt)["Legend"]);
        }

        m_ActiveScene->Initialize(this);
    }

    void Game::TanksGame::Start()
    {
        checkMsg(m_ResourceManager != nullptr, "Can't initiate resources");

        const nlohmann::json SceneConfig = m_ResourceManager->GetJsonConfig("MainScene", ResourceType::Scene);

        CreateNewScene(SceneConfig, m_Players);

        for (auto Player : m_Players)
        {
            if (auto PlayerHealth = Player->GetOwner()->GetComponentWeak<HealthComponent>().lock())
            {

            }
        }
        
        NextStage();

        if (GameStarted)
        {
            GameStarted->Invoke();
        }
    }
    
    void TanksGame::UnInitialize()
    {
        printf("UnInitialize TankGame");
        GameModeBase::UnInitialize();

       // m_CurrentStageWidget.reset();
    }

    void Game::TanksGame::NextStage()
    {
        m_CurrentStage++;

        const auto text = "Stage: " + std::to_string(m_CurrentStage);
       // m_CurrentStageWidget->Update(text);
    }
}
