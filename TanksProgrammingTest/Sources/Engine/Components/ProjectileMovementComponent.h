﻿#pragma once
#include "Vector2D.h"
#include "Components/EntityComponent.h"

namespace Engine
{
    using namespace std;

    class ProjectileMovementComponent : public EntityComponent
    {
    public:
        ProjectileMovementComponent() :
            EntityComponent(nullptr)
        {
        }

        ProjectileMovementComponent(Entity* Owner):
            EntityComponent(Owner)
        {
        }

        ~ProjectileMovementComponent() override = default;

        unique_ptr<EntityComponent> Clone() const override { return make_unique<ProjectileMovementComponent>(*this); }

        void Update(float DeltaTime) override;
        void Initialize(Scene* Scene) override;

        void SetVelocity(Vector2D<int> Velocity);
        weak_ptr<PhysicsComponent> m_WeakPhysicsComponent;
        Vector2D<int> m_Velocity;
    };
}
