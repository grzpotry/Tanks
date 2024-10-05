#pragma once
#include "Components/EntityComponent.h"

class ProjectileMovementComponent : public EntityComponent
{
public:
    virtual ~ProjectileMovementComponent() = default;
    
    std::unique_ptr<EntityComponent> Clone() const override { return std::make_unique<ProjectileMovementComponent>(*this); }

    void Update(float DeltaTime) override;
    void Initialize() override;

    ProjectileMovementComponent() :
        EntityComponent(nullptr)
    {
    }

    ProjectileMovementComponent(Entity* Owner):
        EntityComponent(Owner)
    {
    }

    void SetVelocity(Vector2D<int> Velocity);
    std::weak_ptr<PhysicsComponent> m_PhysicsComponent;
    Vector2D<int> m_Velocity;
};
