#include "Engine.h"
#include <SDL.h>
#include <stdio.h>
#include "Scene.h"
#include "ResourceManager.h"

Engine::Engine()
	: m_Window(nullptr)
	, m_Renderer(nullptr)
	, m_ActiveScene(nullptr)
	, m_ResourceManager(nullptr)
	, FramesPerSecond(60)
	, TimePerFrameInSeconds(1.0f / FramesPerSecond)
{
}

Engine* Engine::Get()
{
	static Engine s_Instance;
	return &s_Instance;
}

void Engine::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
	{
		printf("Error initializing SDL: %s\n", SDL_GetError());
	}

	m_Window = SDL_CreateWindow("ProgrammingTest", 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		860, 640, 0);

	Uint32 renderFlags = SDL_RENDERER_ACCELERATED;

	m_Renderer = SDL_CreateRenderer(m_Window, -1, renderFlags);

	m_ResourceManager = new ResourceManager("Resources");
	m_ResourceManager->LoadResources();

	TTF_Init();

	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->Initialize();
	}
}

void Engine::Update(float DeltaTime)
{
	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->Update(DeltaTime);
	}
}

void Engine::MainLoop()
{
	bool Close = false;
	unsigned int CurrentTime = SDL_GetTicks();
	double FixedUpdateAccumulator = 0.0f;
	int FrameCount = 0;

	while (!Close)
	{
		const unsigned int NewTime = SDL_GetTicks();
		const unsigned int NextFrameTimeMs = NewTime - CurrentTime;
		double DeltaTime = NextFrameTimeMs /1000.0f;

		CurrentTime = NewTime;
		FixedUpdateAccumulator+= DeltaTime;
		
		SDL_Event Event;
			
		while (SDL_PollEvent(&Event))
		{
			if (Event.type == SDL_QUIT || Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				Close = true;
				break;
			}
			else
			{
				m_Events.emplace_back(Event);
			}
		}

		constexpr int MaxUpdatesPerFrame = 1;
		int UpdateCount = 0;
		
		while (FixedUpdateAccumulator >= TimePerFrameInSeconds && UpdateCount < MaxUpdatesPerFrame)
		{
			Update(TimePerFrameInSeconds );
			FixedUpdateAccumulator -= TimePerFrameInSeconds;
			FrameCount++;
			UpdateCount++;
		}
		
		if (UpdateCount == MaxUpdatesPerFrame)
		{
			FixedUpdateAccumulator = 0.0f;
		}
		
		Draw();

		DeltaTime = (SDL_GetTicks() - NewTime) / 1000.0f;
		
		if (DeltaTime < TimePerFrameInSeconds)
		{
			Uint32 ms = static_cast<Uint32>((TimePerFrameInSeconds - DeltaTime) * 1000.0f);
			//printf("Frame rendered in %i", ms);
			SDL_Delay(ms);
		}

		//printf("frame %i \n", FrameCount);
		m_Events.clear();
	}
}

void Engine::Draw()
{
	SDL_RenderClear(m_Renderer);//back to draw

	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->Draw();
	}

	SDL_RenderPresent(m_Renderer);
}

void Engine::ShutDown()
{
	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->UnInitialize();
	}

	TTF_Quit();
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

void Engine::CreateActiveSceneFromTemplate(std::string Name)
{
	if (m_ResourceManager != nullptr)
	{
		nlohmann::json SceneConfig = m_ResourceManager->GetJsonConfig(Name, ResourceType::Scene);
		Scene* SceneFromTemplate = new Scene();
		SceneFromTemplate->LoadFromConfig(SceneConfig);
		SceneFromTemplate->Initialize();

		m_ActiveScene = SceneFromTemplate;
	}
}

int Engine::QueryCollisions(SDL_Rect SourceRect, std::shared_ptr<PhysicsComponent> const SourceObj) const
{
	return m_ActiveScene->QueryCollisions(SourceRect, SourceObj);
}

void Engine::AddProjectile(Vector2D<int> Position, Vector2D<int> Velocity) const
{
	if (m_ActiveScene)
	{
		m_ActiveScene->AddProjectile(Position, Velocity);
	}
}

