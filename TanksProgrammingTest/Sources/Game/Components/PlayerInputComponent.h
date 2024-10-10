#pragma once
#include "Components/EntityComponent.h"

namespace Game
{
    class TankComponent;
    using namespace std;
    using namespace EngineCore;

    // Handles human player input
    class PlayerInputComponent : public EntityComponent
    {
    public:
        PlayerInputComponent(Entity* Owner);
        PlayerInputComponent();

        [[nodiscard]] unique_ptr<EntityComponent> Clone() const override
        {
            return make_unique<PlayerInputComponent>(*this);
        }

        void Initialize(GameModeBase* Game) override;
        void UnInitialize() override;
        void Update(float DeltaTime) override;

    private:
        weak_ptr<PhysicsComponent> m_WeakPhysicsComponent;
        weak_ptr<TankComponent> m_WeakTankComponent;
    };
}
