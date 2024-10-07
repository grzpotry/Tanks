#include "EnemyControllerComponent.h"
#include "Engine.h"
#include "Entity.h"

namespace Game
{
    EnemyControllerComponent::EnemyControllerComponent(Entity* Owner)
        : EntityComponent(Owner)
    {
    }

    EnemyControllerComponent::EnemyControllerComponent()
        : EnemyControllerComponent(nullptr)
    {
    }

    void EnemyControllerComponent::Initialize(Scene* Scene)
    {
        EntityComponent::Initialize(Scene);

        m_TankComponent = GetOwner()->GetComponentWeak<TankComponent>();
    }

    void EnemyControllerComponent::Update(float DeltaTime)
    {
        bool bMovementSucceed = false;
        
        if (const auto Tank = m_TankComponent.lock())
        {
            bMovementSucceed = Tank->TryMove(m_CurrentDirection, DeltaTime);
        }
        
        m_CurrentDirectionTimer += DeltaTime;
        m_CurrentShootTimer += DeltaTime;

        if (m_CurrentShootTimer > m_CurrentShootInterval)
        {
            Shoot();
        }

        if (!bMovementSucceed || m_CurrentDirectionTimer > m_CurrentChangeDirInterval || m_CurrentDirection.magnitude() == 0)
        {
            ChangeDirection();
        }
    }

    void EnemyControllerComponent::Shoot()
    {
        m_CurrentShootTimer = 0.0f;
        
        std::uniform_real_distribution ShootIntervalDist(ShootInterval.X, ShootInterval.Y);
        const auto RandomGenerator = Engine::Get()->GetRandomGenerator();
        m_CurrentShootInterval = ShootIntervalDist(*RandomGenerator);

        if (const auto Tank = m_TankComponent.lock())
        {
            Tank->ShootProjectile();
        }
    }

    void EnemyControllerComponent::ChangeDirection()
    {
        m_CurrentDirectionTimer = .0f;
        std::uniform_real_distribution DirectionIntervalDist(ChangeDirectionInterval.X, ChangeDirectionInterval.Y);
        const auto RandomGenerator = Engine::Get()->GetRandomGenerator();
        m_CurrentChangeDirInterval = DirectionIntervalDist(*RandomGenerator);

        std::vector<Vector2D<int>> PossibleDirections;
        std::copy_if(AllDirections.begin(), AllDirections.end(), std::back_inserter(PossibleDirections),[this](const auto& dir) { return dir != m_CurrentDirection; });

        std::uniform_int_distribution<> DirectionsDist(0, PossibleDirections.size() - 1);
        const auto NewDir = PossibleDirections[DirectionsDist(*RandomGenerator)];

        m_CurrentDirection = NewDir;
    }
}
