#pragma once

#include "Engine.h"
#include "Components/EntityComponent.h"

namespace Game
{
    class TankComponent;
    using namespace std;
    using namespace Engine;
    using Engine = ::Engine::Engine;

    class PlayerInputComponent : public EntityComponent
    {
    public:
        PlayerInputComponent(Entity* Owner);
        PlayerInputComponent();

        [[nodiscard]] unique_ptr<EntityComponent> Clone() const override
        {
            return make_unique<PlayerInputComponent>(*this);
        }

        void Initialize(Scene* Scene) override;
        void UnInitialize() override;
        void Update(float DeltaTime) override;

    private:
        weak_ptr<PhysicsComponent> m_WeakPhysicsComponent;
        weak_ptr<TankComponent> m_WeakTankComponent;
    };
}
