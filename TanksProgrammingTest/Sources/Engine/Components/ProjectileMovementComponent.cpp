#include "ProjectileMovementComponent.h"
#include "Entity.h"
#include "Components/PhysicsComponent.h"

namespace Engine
{
    void ProjectileMovementComponent::Initialize(Scene* const Scene)
    {
        EntityComponent::Initialize(Scene);
        m_WeakPhysicsComponent = GetOwner()->GetComponentWeak<PhysicsComponent>();
    }

    void ProjectileMovementComponent::SetVelocity(Vector2D<int> Velocity)
    {
        m_Velocity = Velocity;
    }

    void ProjectileMovementComponent::Update(float DeltaTime)
    {
        if (const auto PhysicsComponent = m_WeakPhysicsComponent.lock())
        {
            const auto Rect = PhysicsComponent->GetRectTransform();
            PhysicsComponent->SetPosition(Rect.x + static_cast<int>(m_Velocity.X * DeltaTime),
                                          Rect.y + static_cast<int>(m_Velocity.Y * DeltaTime));
        }
    }
}