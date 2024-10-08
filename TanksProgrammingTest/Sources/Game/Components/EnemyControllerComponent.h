#pragma once

#include "Engine.h"
#include "TankComponent.h"
#include "Components/EntityComponent.h"

namespace Game
{
    using namespace EngineCore;

    using namespace std;

    class EnemyControllerComponent : public EntityComponent
    {
    public:
        EnemyControllerComponent(Entity* Owner);
        EnemyControllerComponent();

        void Initialize(GameModeBase* Game) override;
        void Update(float DeltaTime) override;
        void Shoot();

        [[nodiscard]] std::unique_ptr<EntityComponent> Clone() const override
        {
            return make_unique<EnemyControllerComponent>(*this);
        }

    private:
        inline static std::list<Vector2D<int>> AllDirections = {Vector2D(0, 1), Vector2D(0, -1), Vector2D(1, 0), Vector2D(-1, 0)};
        
        const Vector2D<float> ChangeDirectionInterval = Vector2D(0.3f, 1.5f);
        const Vector2D<float> ShootInterval = Vector2D(0.2f, 1.1f);
        
        weak_ptr<TankComponent> m_TankComponent;
        float m_CurrentChangeDirInterval = 0.0f;
        float m_CurrentShootInterval = 1.0f;
        float m_CurrentDirectionTimer = 0.0f;
        float m_CurrentShootTimer = 0.0f;
        Vector2D<int> m_CurrentDirection;
        
        void ChangeDirection();
    };
}
