#include "PlayerInputComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "../Config.h"
#include "Components/PhysicsComponent.h"

namespace Game
{
    using namespace Engine;
    using Engine = ::Engine::Engine;
    
    PlayerInputComponent::PlayerInputComponent(Entity* Owner)
        : EntityComponent(Owner)
    {
    }

    PlayerInputComponent::PlayerInputComponent()
        : PlayerInputComponent(nullptr)
    {
    }

    void PlayerInputComponent::Initialize()
    {
        m_WeakPhysicsComponent = GetOwner()->GetComponentWeak<PhysicsComponent>();
    }

    bool TryFindBetterPosition(SDL_Rect SourceRect, const shared_ptr<PhysicsComponent>& SourceObj, SDL_Rect& FixedRect, bool AdjustX)
    {
        const int MaxFix = (AdjustX ? SourceRect.h : SourceRect.w) / 2;

        for (int i = 0; i < MaxFix; i++)
        {
            for (int dir = -1; dir <= 1; dir += 2)
            {
                SDL_Rect Potential = SourceRect;
                
                if (AdjustX)
                {
                    Potential.x += dir * i;
                }
                else
                {
                    Potential.y += dir * i;
                }
                
                if (Engine::Get()->QueryCollisions(Potential, SourceObj) == 0)
                {
                    printf("Adjusted rect position by %i \n", i);
                    FixedRect = Potential;
                    return true;
                }
            }
        }

        return false;
    }

    void PlayerInputComponent::Update(float DeltaTime)
    {
        const int MoveDistance = Config::PlayerSpeed * DeltaTime;

        if (MoveDistance == 0)
        {
            printf("Can't move player, very small delta time [%f].  ", DeltaTime);
        }

        if (const auto PhysicsComponent = m_WeakPhysicsComponent.lock())
        {
            SDL_Rect Rectangle = PhysicsComponent->GetRectTransform();
            const vector<SDL_Event> Events = Engine::Get()->GetEvents();
            const Uint8* Keystates = SDL_GetKeyboardState(NULL);

            //TODO: add some input handling layer
            if (Keystates[SDL_SCANCODE_LEFT] || Keystates[SDL_SCANCODE_A])
            {
                Rectangle.x -= MoveDistance;
                PhysicsComponent->SetRotationAngle(-90);
            }
            else if (Keystates[SDL_SCANCODE_RIGHT] || Keystates[SDL_SCANCODE_D])
            {
                Rectangle.x += MoveDistance;
                PhysicsComponent->SetRotationAngle(90);
            }
            else if (Keystates[SDL_SCANCODE_W] || Keystates[SDL_SCANCODE_UP])
            {
                Rectangle.y -= MoveDistance;
                PhysicsComponent->SetRotationAngle(0);
            }
            else if (Keystates[SDL_SCANCODE_S] || Keystates[SDL_SCANCODE_DOWN])
            {
                Rectangle.y += MoveDistance;
                PhysicsComponent->SetRotationAngle(180);
            }

            const int Collisions = Engine::Get()->QueryCollisions(Rectangle, PhysicsComponent);
            const auto PlayerRect = &PhysicsComponent->GetRectTransform();
            
            if (Collisions > 0)
            {
                SDL_Rect OutAdjustedRect;
                const int Angle = PhysicsComponent->GetRotationAngle();
                const bool AdjustX = Angle == 0 || Angle == 180;

                if (TryFindBetterPosition(Rectangle, PhysicsComponent, OutAdjustedRect, AdjustX))
                {
                    PlayerRect->x = OutAdjustedRect.x;
                    PlayerRect->y = OutAdjustedRect.y;
                }
            }
            else
            {
                PlayerRect->x = Rectangle.x;
                PlayerRect->y = Rectangle.y;
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
                                const auto Velocity = PhysicsComponent->GetForward() * Config::ProjectileSpeed;
                            
                                const Vector2D<int> StartOffset = Vector2D(10, 10) + PhysicsComponent->GetForward() * 10;
                                Engine::Get()->AddProjectile(Vector2D(Rectangle.x + StartOffset.X, Rectangle.y + StartOffset.Y),Velocity, GetOwner());
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
            }


            int MaxWidth = 0, MaxHeight = 0;
            SDL_GetWindowSize(Engine::Get()->GetWindow(), &MaxWidth, &MaxHeight);

            //restrict movement
            if (Rectangle.x + Rectangle.w > MaxWidth)
            {
                Rectangle.x = MaxWidth - Rectangle.w;
            }

            if (Rectangle.x < 0)
            {
                Rectangle.x = 0;
            }

            if (Rectangle.y + Rectangle.h > MaxHeight)
            {
                Rectangle.y = MaxHeight - Rectangle.h;
            }

            if (Rectangle.y < 0)
            {
                Rectangle.y = 0;
            }
        }
    }
    
}