#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

class PhysicsComponent;
class Scene;
class ResourceManager;

class Engine
{

public:
	static Engine* Get();

	void Initialize();
	void Update(float DeltaTime);
	void MainLoop();
	void Draw();
	void ShutDown();

	SDL_Renderer* GetRenderer() { return m_Renderer; }
	SDL_Window* GetWindow() { return m_Window; }
	ResourceManager* GetResourceManager() { return m_ResourceManager; }
	const std::vector<SDL_Event>& GetEvents() { return m_Events; }
	void SetActiveScene(Scene* Scene) { m_ActiveScene = Scene; }
	void CreateActiveSceneFromTemplate(std::string Name);

	int QueryCollisions(SDL_Rect SourceRect, PhysicsComponent* const SourceObj) const;

private:
	Engine();

	SDL_Window* m_Window;
	SDL_Renderer* m_Renderer;
	Scene* m_ActiveScene;
	ResourceManager* m_ResourceManager;
	
	unsigned int FramesPerSecond;
	float TimePerFrameInSeconds;

	std::vector<SDL_Event> m_Events;
};