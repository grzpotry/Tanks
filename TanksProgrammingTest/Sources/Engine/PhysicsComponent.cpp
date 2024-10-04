#include "PhysicsComponent.h"

void PhysicsComponent::OnCollision(PhysicsComponent* Other)
{
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

void PhysicsComponent::SetRotationAngle(float angle)
{
    m_RotationAngle = angle;
}