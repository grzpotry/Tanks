#include "PlayerInputComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "Scene.h"
#include "TankComponent.h"
#include "Components/PhysicsComponent.h"

namespace Game
{
    PlayerInputComponent::PlayerInputComponent(Entity* Owner)
        : EntityComponent(Owner)
    {
    }

    PlayerInputComponent::PlayerInputComponent()
        : PlayerInputComponent(nullptr)
    {
    }

    void PlayerInputComponent::Initialize(Scene* const Scene)
    {
        EntityComponent::Initialize(Scene);
        m_WeakPhysicsComponent = GetOwner()->GetComponentWeak<PhysicsComponent>();
        m_WeakTankComponent = GetOwner()->GetComponentWeak<TankComponent>();
    }

    void PlayerInputComponent::UnInitialize()
    {
        m_Scene = nullptr;
        m_WeakPhysicsComponent.reset();
        m_WeakTankComponent.reset();
    }

    void PlayerInputComponent::Update(float DeltaTime)
    {
        if (m_Scene == nullptr)
        {
            return;
        }

        if (const auto PhysicsComponent = m_WeakPhysicsComponent.lock())
        {
            if (const auto TankComponent = m_WeakTankComponent.lock())
            {
                SDL_Rect Rectangle = PhysicsComponent->GetRectTransform();
                const vector<SDL_Event> Events = Engine::Get()->GetEvents();
                const Uint8* Keystates = SDL_GetKeyboardState(NULL);

                //TODO: add some input handling layer
                if (Keystates[SDL_SCANCODE_LEFT] || Keystates[SDL_SCANCODE_A])
                {
                    TankComponent->TryMove(Vector2D(-1, 0), DeltaTime);
                }
                else if (Keystates[SDL_SCANCODE_RIGHT] || Keystates[SDL_SCANCODE_D])
                {
                    TankComponent->TryMove(Vector2D(1, 0), DeltaTime);
                }
                else if (Keystates[SDL_SCANCODE_W] || Keystates[SDL_SCANCODE_UP])
                {
                    TankComponent->TryMove(Vector2D(0, -1), DeltaTime);
                }
                else if (Keystates[SDL_SCANCODE_S] || Keystates[SDL_SCANCODE_DOWN])
                {
                    TankComponent->TryMove(Vector2D(0, 1), DeltaTime);
                }

                for (const SDL_Event& Event : Events)
                {
                    switch (Event.type)
                    {
                    case SDL_KEYDOWN:
                        {
                            switch (Event.key.keysym.scancode)
                            {
                            case SDL_SCANCODE_SPACE:
                                {
                                    TankComponent->ShootProjectile();
                                }
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
