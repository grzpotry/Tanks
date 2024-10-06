#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Entity.h"
#include "Vector2D.h"

#define DEBUG_COLLISIONS true

namespace Engine
{
    using namespace std;

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

        Vector2D<int> GetWindowSize() const { return Vector2D(m_WindowWidth, m_WindowHeight);}
        SDL_Renderer* GetRenderer() const { return m_Renderer; }
        SDL_Window* GetWindow() const { return m_Window; }
        ResourceManager* GetResourceManager() const { return m_ResourceManager; }
        const vector<SDL_Event>& GetEvents() { return m_Events; }
        void SetActiveScene(Scene* Scene) { m_ActiveScene = Scene; }
        void CreateActiveSceneFromTemplate(string Name);

        int QueryCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const SourceObj) const;
        void AddProjectile(Vector2D<int> Position, Vector2D<int> Velocity) const;

    private:
        Engine();

        SDL_Window* m_Window;
        SDL_Renderer* m_Renderer;
        Scene* m_ActiveScene;
        ResourceManager* m_ResourceManager;

        int m_WindowWidth;
        int m_WindowHeight;

        unsigned int FramesPerSecond;
        float TimePerFrameInSeconds;

        vector<SDL_Event> m_Events;
    };
}
