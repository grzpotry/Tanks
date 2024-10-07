#pragma once
#include <iostream>
#include <SDL_rect.h>

#include "EngineUtils.h"
#include "EntityComponent.h"
#include "Vector2D.h"

namespace Engine
{
    using namespace std;

    class PhysicsComponent : public EntityComponent
    {
    public:

        PhysicsComponent();
        PhysicsComponent(Entity* Owner);

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

        void Update(float DeltaTime) override;
        void OnCollision(const shared_ptr<PhysicsComponent>& Other) const;
        void LoadFromConfig(nlohmann::json Config) override;
        unique_ptr<EntityComponent> Clone() const override { return make_unique<PhysicsComponent>(*this); }
    
        void SetPosition(int x, int y);
        void SetScale(int w, int h);
        void SetRotationAngle(int EulerDeg);
        void SelfDestroyOutsideWindow() const;

        [[nodiscard]] SDL_Rect& GetRectTransform() { return m_RectTransform; }
        [[nodiscard]] int GetRotationAngle() const {return m_RotationAngle;}
        [[nodiscard]] bool IsStatic() const {return m_IsStatic;}
    
        [[nodiscard]] SDL_Point GetCenter() const
        {
            SDL_Point Center;
            Center.x = m_RectTransform.w / 2;
            Center.y = m_RectTransform.h / 2;
            return Center;
        }

        [[nodiscard]] Vector2D<int> GetForward() const { return m_Forward; }
        [[nodiscard]] uint8_t GetCollisionLayer() const { return m_CollisionLayer; }
        [[nodiscard]] uint8_t GetDestroyOnCollisionMask() const { return m_DestroyOnCollisionMask; }
    
    private:
        bool m_IsStatic;
        SDL_Rect m_RectTransform;
        int m_RotationAngle = 0;

        uint8_t m_DestroyOnCollisionMask;
        uint8_t m_CollisionLayer;

        Vector2D<int> m_Forward;
    };
}