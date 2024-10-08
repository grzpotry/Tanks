#pragma once

#include <random>
#include <SDL.h>
#include <vector>
#include "Entity.h"
#include "GUI.h"
#include "Vector2D.h"

#define DEBUG_COLLISIONS false

namespace EngineCore
{
    class GameModeBase;
    using namespace std;

    class PhysicsComponent;
    class ResourceManager;

    class Engine
    {
    public:
        static Engine* Get();

        void Initialize();
        void Update(float DeltaTime) const;
        void MainLoop();
        void Draw();
        void ShutDown();

        Vector2D<int> GetWindowSize() const { return {m_WindowWidth, m_WindowHeight};}
        [[nodiscard]] SDL_Renderer* GetRenderer() const { return m_Renderer; }
        SDL_Window* GetWindow() const { return m_Window; }
        ResourceManager* GetResourceManager() const { return m_ResourceManager; }
        const vector<SDL_Event>& GetEvents() { return m_Events; }
        shared_ptr<std::mt19937> GetRandomGenerator() const {return m_RandomGenerator;}
        
        void StartGame(GameModeBase* Game);

        void BindGUI(unique_ptr<GUI> GUI);
    
    private:
        Engine();

        SDL_Window* m_Window;
        unique_ptr<GUI> m_GUI;
        SDL_Renderer* m_Renderer;
        GameModeBase* m_ActiveGame;
        ResourceManager* m_ResourceManager;

        int m_WindowWidth;
        int m_WindowHeight;

        unsigned int FramesPerSecond;
        float TimePerFrameInSeconds;

        vector<SDL_Event> m_Events;
        
        std::random_device RandomDevice;
        shared_ptr<std::mt19937>  m_RandomGenerator;
    };
}
