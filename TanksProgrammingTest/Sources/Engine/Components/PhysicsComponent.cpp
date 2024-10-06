#include "PhysicsComponent.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "Entity.h"
#include <cassert>
#include <iostream>

namespace Engine
{
    void PhysicsComponent::Update(float DeltaTime)
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

    void PhysicsComponent::OnCollision(std::shared_ptr<PhysicsComponent> Other)
    {
        if (GetOwner()->IsPendingDestroy() || Other->GetOwner()->IsPendingDestroy())
        {
            return;
        }
        if (GetOwner()->GetName() == "Projectile" && Other->GetOwner()->GetName() != "Player")
        {
            Other->GetOwner()->MarkDestroy();
            GetOwner()->MarkDestroy();
            // printf("projectile collision");
        }
    }

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
}