#include "PlayerInputComponent.h"
#include "Entity.h"
#include "Engine.h"
#include "Components/PhysicsComponent.h"

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
    m_PhysicsComponent = GetOwner()->GetComponentWeak<PhysicsComponent>();
}

void PlayerInputComponent::Update(float DeltaTime)
{
    constexpr int Speed = 400;
    const int MoveDistance = Speed * DeltaTime;

    if (MoveDistance == 0)
    {
        printf("Can't move player, very small delta time [%f].  ", DeltaTime);
    }

    if (const auto PhysicsComponent = m_PhysicsComponent.lock())
    {
        SDL_Rect Rectangle = PhysicsComponent->GetRectTransform();
        std::vector<SDL_Event> Events = Engine::Get()->GetEvents();
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

        const int Collisions = Engine::Get()->QueryCollisions(Rectangle, PhysicsComponent); //avoid get
        if (Collisions > 0)
        {
            //can't move player
        }
        else
        {
            const auto PlayerRect = &PhysicsComponent->GetRectTransform();
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
                            constexpr int ProjectileSpeed = 400;
                            const auto velocity = PhysicsComponent->GetForward();
                            Vector2D<int> VelocityInt(velocity.X, velocity.Y);
                            Engine::Get()->AddProjectile(Vector2D(Rectangle.x + (Rectangle.w - 5) / 2, Rectangle.y),
                                                         VelocityInt * ProjectileSpeed);
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
