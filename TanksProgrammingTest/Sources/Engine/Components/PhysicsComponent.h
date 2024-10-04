#pragma once
#include <SDL_rect.h>
#include "EntityComponent.h"


class PhysicsComponent : public EntityComponent
{
public:

    PhysicsComponent();
    PhysicsComponent(Entity* Owner);
    
    virtual ~PhysicsComponent() = default;

    PhysicsComponent(const PhysicsComponent& other)
        : EntityComponent(other),
          m_IsStatic(other.m_IsStatic),
          m_RectTransform(other.m_RectTransform),
          m_RotationAngle(other.m_RotationAngle)
    {
    }

    PhysicsComponent(PhysicsComponent&& other) noexcept
       : EntityComponent(static_cast<EntityComponent&>(other)),                      
         m_IsStatic(other.m_IsStatic),
         m_RectTransform(other.m_RectTransform),
         m_RotationAngle(other.m_RotationAngle)
    {
    }

    PhysicsComponent& operator=(const PhysicsComponent& other)
    {
        if (this == &other)
            return *this;
        EntityComponent::operator =(other);
        m_IsStatic = other.m_IsStatic;
        m_RectTransform = other.m_RectTransform;
        m_RotationAngle = other.m_RotationAngle;
        return *this;
    }

    PhysicsComponent& operator=(PhysicsComponent&& other) noexcept
    {
        if (this == &other)
            return *this;
        EntityComponent::operator =(other);
        m_IsStatic = other.m_IsStatic;
        m_RectTransform = other.m_RectTransform;
        m_RotationAngle = other.m_RotationAngle;
        return *this;
    }

    void OnCollision(PhysicsComponent* Other);
    void LoadFromConfig(nlohmann::json Config) override;
    EntityComponent* Clone() const override { return new PhysicsComponent(*this); }
    
    void SetPosition(int x, int y);
    void SetScale(int w, int h);
    void SetRotationAngle(float EulerDeg);

    [[nodiscard]] SDL_Rect& GetRectTransform() { return m_RectTransform; }
    [[nodiscard]] float GetRotationAngle() const {return m_RotationAngle;}
    [[nodiscard]] bool IsStatic() const {return m_IsStatic;}
    
    [[nodiscard]] SDL_Point GetCenter() const
    {
        SDL_Point Center;
        Center.x = m_RectTransform.w / 2;
        Center.y = m_RectTransform.h / 2;
        return Center;
    }

    Vector2D<float> GetForward() const { return m_Forward; }
    
private:
    bool m_IsStatic;
    SDL_Rect m_RectTransform;
    float m_RotationAngle = 0.0;

    Vector2D<float> m_Forward;
};
