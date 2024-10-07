#include "Engine.h"
#include "ResourceManager.h"
#include "Components/EnemyControllerComponent.h"
#include "Components/TextureComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/TankComponent.h"

using namespace Engine;
using EngineClass = ::Engine::Engine;
using namespace Game;

int main(int argc, char* argv[])
{
    EngineClass::Get()->Initialize();

    ResourceManager* ResourceManagerPtr = EngineClass::Get()->GetResourceManager();
    ResourceManagerPtr->RegisterComponent("PlayerInputComponent", new PlayerInputComponent());
    ResourceManagerPtr->RegisterComponent("TextureComponent", new TextureComponent());
    ResourceManagerPtr->RegisterComponent("PhysicsComponent", new PhysicsComponent());
    ResourceManagerPtr->RegisterComponent("ProjectileMovementComponent", new ProjectileMovementComponent());
    ResourceManagerPtr->RegisterComponent("EnemyControllerComponent", new EnemyControllerComponent());
    ResourceManagerPtr->RegisterComponent("TankComponent", new TankComponent());

    EngineClass::Get()->CreateActiveSceneFromTemplate("MainScene");
    EngineClass::Get()->MainLoop();
    EngineClass::Get()->ShutDown();

    return 0;
}
