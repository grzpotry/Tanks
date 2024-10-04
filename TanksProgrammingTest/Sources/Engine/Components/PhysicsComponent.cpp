#include "PhysicsComponent.h"

#include "Engine.h"
#include "Entity.h"

void PhysicsComponent::OnCollision(PhysicsComponent* Other)
{
    if (GetOwner()->IsMarkedToDestroy())
    {
        return;
    }
    if (GetOwner()->GetName() == "Projectile" && Other->GetOwner()->GetName() != "Player")
    {
        Other->GetOwner()->Destroy();
        GetOwner()->Destroy();
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

    if (!m_IsStatic)
    {
        printf("IsStatic: %i \n", m_IsStatic);
    }
   
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

void PhysicsComponent::SetRotationAngle(float EulerDeg)
{
    m_RotationAngle = EulerDeg;
    
    const float PI = 3.14159265f;
    const float AngleRad = EulerDeg * (PI/ 180.0f);
    
    float x = std::sin(AngleRad);
    float y = std::cos(AngleRad);

    m_Forward = Vector2D(x, -y);
}