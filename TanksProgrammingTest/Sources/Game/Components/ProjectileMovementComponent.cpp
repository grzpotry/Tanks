#include "ProjectileMovementComponent.h"
#include "Entity.h"
#include "Components/PhysicsComponent.h"

void ProjectileMovementComponent::Initialize()
{
    m_PhysicsComponent = GetOwner()->GetComponentWeak<PhysicsComponent>();
}

void ProjectileMovementComponent::SetVelocity(Vector2D<int> Velocity)
{
    m_Velocity = Velocity;
}

void ProjectileMovementComponent::Update(float DeltaTime)
{
    if (const auto PhysicsComponent = m_PhysicsComponent.lock()) {
        auto Rect = PhysicsComponent->GetRectTransform();
        PhysicsComponent->SetPosition(Rect.x + (int)(m_Velocity.X * DeltaTime), Rect.y + (int)(m_Velocity.Y * DeltaTime));
    }
}
