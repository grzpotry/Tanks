#include "ProjectileMovementComponent.h"
#include "Entity.h"
#include "Components/PhysicsComponent.h"

void ProjectileMovementComponent::Initialize()
{
    m_PhysicsComponent = GetOwner()->GetComponent<PhysicsComponent>();
}

void ProjectileMovementComponent::SetVelocity(Vector2D<int> Velocity)
{
    m_Velocity = Velocity;
}

void ProjectileMovementComponent::Update(float DeltaTime)
{
    auto Rect = m_PhysicsComponent->GetRectTransform();
    m_PhysicsComponent->SetPosition(Rect.x + (int)(m_Velocity.X * DeltaTime), Rect.y + (int)(m_Velocity.Y * DeltaTime));
}
