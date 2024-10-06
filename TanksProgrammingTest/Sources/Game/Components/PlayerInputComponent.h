#pragma once

#include "Components/EntityComponent.h"

namespace Game
{
    using namespace std;

    class PlayerInputComponent : public Engine::EntityComponent
    {
    public:
        PlayerInputComponent(Engine::Entity* Owner);
        PlayerInputComponent();

        [[nodiscard]] unique_ptr<EntityComponent> Clone() const override { return make_unique<PlayerInputComponent>(*this); }

        void Initialize() override;
        void Update(float DeltaTime) override;

    private:
        weak_ptr<Engine::PhysicsComponent> m_WeakPhysicsComponent;
    };
}