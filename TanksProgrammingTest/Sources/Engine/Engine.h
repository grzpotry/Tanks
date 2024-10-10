#pragma once

#include <random>
#include <SDL.h>
#include <vector>
#include "Entity.h"
#include "GUI.h"
#include "Vector2D.h"

#define DEBUG_COLLISIONS false
#define ENABLE_PROFILING false

namespace EngineCore
{
    using namespace std;
    
    class GameModeBase;
    class PhysicsComponent;
    class ResourceManager;

    // game engine, handles rendering and main game loop
    class Engine
    {
    public:
        static Engine* Get();

        void Initialize();
        void Update(float DeltaTime) const;
        void MainLoop();
        void Draw();
        void ShutDown();

        [[nodiscard]] Vector2D<int> GetWindowSize() const { return {m_WindowWidth, m_WindowHeight}; }
        [[nodiscard]] SDL_Renderer* GetRenderer() const { return m_Renderer; }
        [[nodiscard]] SDL_Window* GetWindow() const { return m_Window; }
        [[nodiscard]] shared_ptr<ResourceManager> GetResourceManager() const { return m_ResourceManager; }
        [[nodiscard]] const vector<SDL_Event>& GetEvents() { return m_Events; }
        [[nodiscard]] shared_ptr<std::mt19937> GetRandomGenerator() const { return m_RandomGenerator; }
        
        void StartGame(GameModeBase* Game);
        void BindGUI(unique_ptr<GUI> GUI);
    
    private:
        Engine();

        int m_WindowWidth = 0;
        int m_WindowHeight = 0;
        unsigned int FramesPerSecond;
        float TimePerFrameInSeconds;

        SDL_Window* m_Window;
        unique_ptr<GUI> m_GUI;
        SDL_Renderer* m_Renderer = nullptr;
        GameModeBase* m_ActiveGame = nullptr;
        shared_ptr<ResourceManager> m_ResourceManager = nullptr;
        vector<SDL_Event> m_Events;
        std::random_device RandomDevice;
        shared_ptr<std::mt19937>  m_RandomGenerator;
    };
}
