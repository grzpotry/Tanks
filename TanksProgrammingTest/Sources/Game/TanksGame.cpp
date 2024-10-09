#include "TanksGame.h"

#include "EngineUtils.h"
#include "Components/HealthComponent.h"

namespace Game
{
    void TanksGame::CreateDefaultEntities(nlohmann::json Config) const
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

                Vector2D<int> StartPosition = Vector2D(0,0);

                if (const auto Physics = NewEntity->GetComponentWeak<PhysicsComponent>().lock())
                {
                    const auto Transform = Physics->GetRectTransform();
                    StartPosition = Vector2D(Transform.x, Transform.y); 
                
                }

                m_ActiveScene->AddEntity(std::move(NewEntity), StartPosition);
            }
        }
    }

    void TanksGame::CreateNewScene(nlohmann::json Config)
    {
        const auto basic_string = Config.value("Name", "");
        const float TimeToVictory = Config.value("TimeToVictory", 0.0f);
        m_ActiveScene = new Scene(std::move(basic_string), TimeToVictory);
        
        printf("LoadSceneFromConfig \n");

        CreateDefaultEntities(Config);

        if (const nlohmann::json::const_iterator SceneLayoutIt = Config.find("SceneLayout"); SceneLayoutIt != Config.end())
        {
            m_ActiveScene->LoadSceneFromLayout((*SceneLayoutIt)["Content"], (*SceneLayoutIt)["Legend"], (*SceneLayoutIt)["PositionMarkers"]);
        }

        m_ActiveScene->Initialize(this);
    }

    void TanksGame::Start()
    {
        checkMsg(m_ResourceManager != nullptr, "Can't initiate resources");

        const nlohmann::json SceneConfig = m_ResourceManager->GetJsonConfig("MainScene", ResourceType::Scene);

        CreateNewScene(SceneConfig);

        const auto entity = m_ActiveScene->AddPlayer();
        m_Players.push_back(entity);

        if (GameStarted)
        {
            GameStarted->Invoke();
        }

        NextStage();
    }
    
    void TanksGame::UnInitialize()
    {
        printf("UnInitialize TankGame");
        GameModeBase::UnInitialize();

    }

    void TanksGame::NextStage()
    {
        m_CurrentStage++;

        if (StageChanged)
        {
            StageChanged->Invoke();
        }
    }
}
