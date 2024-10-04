#pragma once
#include <SDL_rect.h>

#include "EntityComponent.h"

class PhysicsComponent : public EntityComponent
{
public:
    PhysicsComponent(const PhysicsComponent& other)
        : EntityComponent(other),
          m_IsStatic(other.m_IsStatic),
          m_RectTransform(other.m_RectTransform),
          m_RotationAngle(other.m_RotationAngle)
    {
    }

    PhysicsComponent(PhysicsComponent&& other) noexcept
        : EntityComponent(std::move(other)),
          m_IsStatic(other.m_IsStatic),
          m_RectTransform(std::move(other.m_RectTransform)),
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
        m_IsStatic = other.m_IsStatic;
        m_RectTransform = other.m_RectTransform;
        m_RotationAngle = other.m_RotationAngle;

        // EntityComponent::operator =(other);
        return *this;
    }

    void OnCollision(PhysicsComponent* Other);

    PhysicsComponent();
    virtual ~PhysicsComponent() = default;
    PhysicsComponent(Entity* Owner);
    virtual void LoadFromConfig(nlohmann::json Config) override;
    virtual EntityComponent* Clone() const override { return new PhysicsComponent(*this); }
    
    void SetPosition(int x, int y);
    void SetScale(int w, int h);
    void SetRotationAngle(float angle);

    SDL_Rect& GetRectTransform() { return m_RectTransform; }
    [[nodiscard]] constexpr float GetRotationAngle() const {return m_RotationAngle;}
    [[nodiscard]] constexpr bool IsStatic() const {return m_IsStatic;}
    
    SDL_Point GetCenter() const
    {
        SDL_Point Center;
        Center.x = m_RectTransform.w / 2;
        Center.y = m_RectTransform.h / 2;
        return Center;

        
    }
    
private:
    bool m_IsStatic;
    SDL_Rect m_RectTransform;
    float m_RotationAngle = 0.0;
    
};
