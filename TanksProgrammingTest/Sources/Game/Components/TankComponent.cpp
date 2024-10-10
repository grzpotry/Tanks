#include "TankComponent.h"
#include "EngineUtils.h"
#include "GameModeBase.h"
#include "Components/PhysicsComponent.h"

namespace Game
{
    using namespace EngineCore;
    
    TankComponent::TankComponent(Entity* Owner)
        : EntityComponent(Owner)
    {
    }

    TankComponent::TankComponent() : TankComponent(nullptr)
    {
    }

    void TankComponent::TryShootProjectile()
    {
        if (IsShootCooldownActive())
        {
            return;
        }

        m_ShootCooldownTimer = ShootCooldown;
        
        if (const auto Physics = m_Physics.lock())
        {
            const auto Direction = Physics->GetForward();
            const SDL_Rect Rectangle = Physics->GetRectTransform();
                            
            const Vector2D<int> StartOffset = Vector2D(10, 10) + Physics->GetForward() * 10;
            m_Game->GetActiveScene()->AddProjectile(Vector2D(Rectangle.x + StartOffset.X, Rectangle.y + StartOffset.Y),Direction, GetOwner());
        }
    }

    void TankComponent::Update(float DeltaTime)
    {
        if (IsShootCooldownActive())
        {
            m_ShootCooldownTimer -= DeltaTime;
        }
    }

    bool TankComponent::TryMove(Vector2D<int> Direction, float DeltaTime) const
    {
        if (Direction.magnitude() == 0)
        {
            return false;
        }
        
        checkMsg(Direction.magnitude() == 1, "Direction vector must be normalized");
        
        if (const auto Physics = m_Physics.lock())
        {
            SDL_Rect Rectangle = Physics->GetRectTransform();
            
            if (Direction.X > 0)
            {
                Physics->SetRotationAngle(90);
            }
            else if (Direction.X < 0)
            {
                Physics->SetRotationAngle(-90);
            }
            else if (Direction.Y < 0)
            {
                Physics->SetRotationAngle(0);
            }
            else if (Direction.Y > 0)
            {
                Physics->SetRotationAngle(180);
            }

            const int MoveDistance = Physics->GetMovementSpeed() * DeltaTime;

            if (MoveDistance == 0)
            {
                printf("Can't move player, very small delta time [%f].  ", DeltaTime);
            }

            Rectangle.x += Direction.X * MoveDistance;
            Rectangle.y += Direction.Y * MoveDistance;
            
            const int Collisions = m_Game->GetActiveScene()->QueryCollisions(Rectangle, Physics, true, true, CollisionLayer::WallsAndTanks);
            const auto PhysicsRect = &Physics->GetRectTransform();

            bool bSuccess = Collisions == 0;
            
            if (!bSuccess)
            {
                SDL_Rect OutAdjustedRect;
                const bool AdjustX = Direction.Y != 0;

                // if we collide slightly, try find nearest valid position for better movement feeling
                if (m_Game->GetActiveScene()->TryFindBetterMovePosition(Rectangle, Physics, OutAdjustedRect, AdjustX))
                {
                    Rectangle.x = OutAdjustedRect.x;
                    Rectangle.y = OutAdjustedRect.y;

                    bSuccess = true;
                }
            }
            
            if (bSuccess)
            {
                PhysicsRect->x = Rectangle.x;
                PhysicsRect->y = Rectangle.y;
            }
            
            int MaxWidth = 0, MaxHeight = 0;
            SDL_GetWindowSize(Engine::Get()->GetWindow(), &MaxWidth, &MaxHeight);

            //restrict movement
            if (PhysicsRect->x + PhysicsRect->w > MaxWidth)
            {
                PhysicsRect->x = MaxWidth - PhysicsRect->w;
            }

            if (PhysicsRect->x < 0)
            {
                PhysicsRect->x = 0;
            }

            if (PhysicsRect->y + PhysicsRect->h > MaxHeight)
            {
                PhysicsRect->y = MaxHeight - PhysicsRect->h;
            }

            if (PhysicsRect->y < 0)
            {
                PhysicsRect->y = 0;
            }

            return bSuccess;
        }
        return false;
    }

    void TankComponent::Initialize(GameModeBase* Game)
    {
        EntityComponent::Initialize(Game);

        m_Physics = GetOwner()->GetComponentWeak<PhysicsComponent>();
    }

    bool TankComponent::IsShootCooldownActive() const
    {
        return m_ShootCooldownTimer > 0;
    }
}
