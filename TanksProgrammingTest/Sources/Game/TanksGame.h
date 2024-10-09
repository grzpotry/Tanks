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
            StageChanged = make_shared<Event<>>();
        }

        ~TanksGame() override
        {
            StageChanged = nullptr;
            GameStarted = nullptr;
            m_Players.clear();
        }

        TanksGame(const TanksGame& other) = delete;

        void CreateDefaultEntities(nlohmann::json Config) const;
        void CreateNewScene(nlohmann::json Config);
        void Start() override;
        void UnInitialize() override;
        
        shared_ptr<Event<>> GameStarted;
        shared_ptr<Event<>> StageChanged;

        std::list<shared_ptr<Entity>>& GetPlayers() {return m_Players;}

        unsigned short GetCurrentStage() const { return m_CurrentStage; }

    private:
        unsigned short m_CurrentStage = 0;

        void NextStage();

        //std::shared_ptr<TextWidget> m_CurrentStageWidget;
        std::list<shared_ptr<Entity>> m_Players;
        //std::unique_ptr<TanksGUI> m_GUI;
    };
}
