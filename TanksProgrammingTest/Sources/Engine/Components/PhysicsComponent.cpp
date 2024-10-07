#include "PhysicsComponent.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "Entity.h"
#include <cassert>
#include <iostream>
#include "CollisionUtils.h"

namespace Engine
{
    PhysicsComponent::PhysicsComponent() :
        EntityComponent(nullptr),
        m_IsStatic(false),
        m_RectTransform()
    {
    }

    PhysicsComponent::PhysicsComponent(Entity* Owner):
        EntityComponent(Owner),
        m_IsStatic(false),
        m_RectTransform()
    {
    }

    void PhysicsComponent::LoadFromConfig(nlohmann::json Config)
    {
        m_IsStatic = Config.value("IsStatic", false);
        m_RectTransform.w = Config.value("Width", 10);
        m_RectTransform.h = Config.value("Height", 10);
        m_RectTransform.x = Config.value("PositionX", 0);
        m_RectTransform.y = Config.value("PositionY", 0);
        
        auto CollisionLayer = Config.value("CollisionLayer", "None");
        auto DestroyCollisionMask = Config.value("DestroyOnCollisionMask", "None");
        
        m_CollisionLayer = CollisionUtils::StringToCollisionMask(CollisionLayer);
        m_DestroyOnCollisionMask = CollisionUtils::StringToCollisionMask(DestroyCollisionMask);

#if DEBUG_COLLISIONS
        printf("CollisionLayer [%s] : %i DestroyMask [%s] : %i \n", CollisionLayer.c_str(), m_CollisionLayer, DestroyCollisionMask.c_str(), m_DestroyOnCollisionMask);
#endif
    }

    void PhysicsComponent::SetPosition(int x, int y)
    {
        m_RectTransform.x = x;
        m_RectTransform.y = y;
    }

    void PhysicsComponent::SetScale(int w, int h)
    {
        m_RectTransform.w = w;
        m_RectTransform.h = h;
    }

    void PhysicsComponent::SetRotationAngle(int EulerDeg)
    {
        check(EulerDeg % 90 == 0, "Unexpected rotation, only 4 basic directions are supported");
    
        m_RotationAngle = EulerDeg;


        constexpr float PI = 3.14159265f;
        const float AngleRad = EulerDeg * (PI / 180.0f);

        // conversion is safe as we assume rotation only in 4 directions, sin/cos will be always 0 or 1
        int X = std::sin(AngleRad);  // NOLINT(bugprone-narrowing-conversions, clang-diagnostic-float-conversion, cppcoreguidelines-narrowing-conversions)
        int Y = std::cos(AngleRad);  // NOLINT(bugprone-narrowing-conversions, clang-diagnostic-float-conversion, cppcoreguidelines-narrowing-conversions)

        // origin is in top-left so invert Y axis to treat "UP screen direction" as UP dir in game
        m_Forward = Vector2D(X, -Y);
    }

    void PhysicsComponent::SelfDestroyOutsideWindow() const
    {
        const auto WindowSize = Engine::Get()->GetWindowSize();
        
        if (m_RectTransform.x> WindowSize.X
            || m_RectTransform.x < 0
            || m_RectTransform.y > WindowSize.Y
            || m_RectTransform.y < 0)
        {
            GetOwner()->MarkDestroy();
        }
    }

    void PhysicsComponent::Update(float DeltaTime)
    {
        SelfDestroyOutsideWindow();
    }

    void PhysicsComponent::OnCollision(const std::shared_ptr<PhysicsComponent>& Other) const
    {
        const auto OtherEntity = Other->GetOwner();
        const auto SelfEntity = GetOwner();
        
        if (SelfEntity->IsPendingDestroy())
        {
            return;
        }

        // avoid self-destruction (eg. from self projectile)
        if (SelfEntity->IsChildOf(OtherEntity) || OtherEntity->IsChildOf(SelfEntity))
        {
            return;
        }

        if (GetDestroyOnCollisionMask() & Other->GetCollisionLayer())
        {
#if DEBUG_COLLISIONS
            printf("auto-destroy %s on collision with %s \n", SelfEntity->GetName().c_str(), OtherEntity->GetName().c_str());
#endif
            SelfEntity->MarkDestroy();
        }
    }
}
