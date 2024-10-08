#pragma once
#include <memory>
#include "Event.h"
#include "GameModeBase.h"

namespace Game
{
    class HealthComponent;
    class PlayerComponent;
    using namespace EngineCore;

    class TanksGame : public GameModeBase
    {
    public:
        TanksGame(Engine* const Engine, ResourceManager* const ResourcesManager)
            : GameModeBase(Engine, ResourcesManager)
        {
            GameStarted = make_shared<Event<>>();
        }

        ~TanksGame() override
        {
            GameStarted = nullptr;
            m_Players.clear();
        }

        TanksGame(const TanksGame& other) = delete;

        void CreateDefaultEntities(nlohmann::json Config, std::list<shared_ptr<PlayerComponent>>& Players) const;
        void CreateNewScene(nlohmann::json Config, std::list<shared_ptr<PlayerComponent>>& Players);
        void Start() override;
        void UnInitialize() override;
        
        shared_ptr<Event<>> GameStarted;

        std::list<shared_ptr<PlayerComponent>>& GetPlayers() {return m_Players;}

    private:
        unsigned short m_CurrentStage = 0;
        unsigned short m_CurrentLives = 3;

        void NextStage();

        //std::shared_ptr<TextWidget> m_CurrentStageWidget;
        std::list<shared_ptr<PlayerComponent>> m_Players;
        //std::unique_ptr<TanksGUI> m_GUI;
    };
}
