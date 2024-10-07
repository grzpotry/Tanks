#pragma once
#include "Engine.h"
#include "Vector2D.h"
#include "Components/EntityComponent.h"

namespace Game
{
    using namespace Engine;
    using Engine = ::Engine::Engine;


    class TankComponent : public EntityComponent
    {
    public:
        TankComponent(Entity* Owner);
        TankComponent();

        [[nodiscard]] std::unique_ptr<EntityComponent> Clone() const override
        {
            return std::make_unique<TankComponent>(*this);
        }

        void ShootProjectile() const;
        bool TryMove(Vector2D<int> Direction, float DeltaTime) const;

        void Initialize(Scene* Scene) override;

    private:
        std::weak_ptr<PhysicsComponent> m_Physics;
    };
}
