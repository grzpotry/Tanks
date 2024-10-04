#include "Engine.h"
#include "ResourceManager.h"
#include "Components/TextureComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/ProjectileMovementComponent.h"

int main(int argc, char* argv[])
{
	Engine::Get()->Initialize();

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	ResourceManagerPtr->RegisterComponent("PlayerInputComponent", new PlayerInputComponent());
	ResourceManagerPtr->RegisterComponent("TextureComponent", new TextureComponent());
	ResourceManagerPtr->RegisterComponent("PhysicsComponent", new PhysicsComponent());
	ResourceManagerPtr->RegisterComponent("ProjectileMovementComponent", new ProjectileMovementComponent());

	Engine::Get()->CreateActiveSceneFromTemplate("MainScene");

	Engine::Get()->MainLoop();
	Engine::Get()->ShutDown();

	return 0;
}
