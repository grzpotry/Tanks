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
    Engine::Get()->Initialize();

    ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
    ResourceManagerPtr->RegisterComponent("PlayerInputComponent", new PlayerInputComponent());
    ResourceManagerPtr->RegisterComponent("TextureComponent", new TextureComponent());
    ResourceManagerPtr->RegisterComponent("PhysicsComponent", new PhysicsComponent());
    ResourceManagerPtr->RegisterComponent("ProjectileMovementComponent", new ProjectileMovementComponent());
    ResourceManagerPtr->RegisterComponent("EnemyControllerComponent", new EnemyControllerComponent());
    ResourceManagerPtr->RegisterComponent("TankComponent", new TankComponent());
    ResourceManagerPtr->RegisterComponent("HealthComponent", new HealthComponent());
    ResourceManagerPtr->RegisterComponent("PlayerComponent", new PlayerComponent());
    ResourceManagerPtr->RegisterComponent("TeamComponent", new TeamComponent());

  
    const auto TanksGame = std::make_shared<Game::TanksGame>(Engine::Get(), Engine::Get()->GetResourceManager());
    auto GUI = make_unique<TanksGUI>(TanksGame);

    Engine::Get()->BindGUI(std::move(GUI));
    
    Engine::Get()->StartGame(TanksGame.get());
    Engine::Get()->MainLoop();
    Engine::Get()->ShutDown();

    return 0;
}
