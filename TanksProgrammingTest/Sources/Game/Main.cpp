#include "Engine.h"
#include "TanksGame.h"
#include "TanksGUI.h"
#include "Components/EnemyControllerComponent.h"
#include "Components/HealthComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/TextureComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/TankComponent.h"
#include "Components/TeamComponent.h"

using namespace EngineCore;
using namespace Game;

int main(int argc, char* argv[])
{
    auto Engine = Engine::Get();
    Engine->Initialize();

    const auto ResourceManagerPtr = Engine->GetResourceManager();
    
    ResourceManagerPtr->RegisterComponent("PlayerInputComponent", new PlayerInputComponent());
    ResourceManagerPtr->RegisterComponent("TextureComponent", new TextureComponent());
    ResourceManagerPtr->RegisterComponent("PhysicsComponent", new PhysicsComponent());
    ResourceManagerPtr->RegisterComponent("ProjectileMovementComponent", new ProjectileMovementComponent());
    ResourceManagerPtr->RegisterComponent("EnemyControllerComponent", new EnemyControllerComponent());
    ResourceManagerPtr->RegisterComponent("TankComponent", new TankComponent());
    ResourceManagerPtr->RegisterComponent("HealthComponent", new HealthComponent());
    ResourceManagerPtr->RegisterComponent("PlayerComponent", new PlayerComponent());
    ResourceManagerPtr->RegisterComponent("TeamComponent", new TeamComponent());
  
    auto TanksGame = std::make_shared<Game::TanksGame>(Engine, Engine->GetResourceManager());
    auto GUI = make_unique<TanksGUI>(TanksGame);

    Engine->BindGUI(std::move(GUI));
    Engine->StartGame(TanksGame.get());
    Engine->MainLoop();
    Engine->ShutDown();

    return 0;
}
