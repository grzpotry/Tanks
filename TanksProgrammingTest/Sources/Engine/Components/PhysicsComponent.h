#pragma once
#include <SDL_rect.h>
#include "EntityComponent.h"
#include "Vector2D.h"

namespace EngineCore
{
    using namespace std;

    // Physical representation in game world, can move and collide
    class PhysicsComponent final : public EntityComponent
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

        unique_ptr<EntityComponent> Clone() const override { return make_unique<PhysicsComponent>(*this); }

        void Update(float DeltaTime) override;
        void OnCollision(const shared_ptr<PhysicsComponent>& Other) const;
        void LoadFromConfig(nlohmann::json Config) override;
        void SelfDestroyOutsideWindow() const;

        void SetPosition(int x, int y);
        void SetScale(int w, int h);
        void SetRotationAngle(int EulerDeg);
        
        [[nodiscard]] int GetMovementSpeed() const { return m_MovementSpeed; }
        [[nodiscard]] SDL_Rect& GetRectTransform() { return m_RectTransform; }
        [[nodiscard]] int GetRotationAngle() const {return m_RotationAngle;}
        [[nodiscard]] bool IsStatic() const {return m_IsStatic;}
        [[nodiscard]] SDL_Point GetCenter() const;
        [[nodiscard]] Vector2D<int> GetForward() const { return m_Forward; }
        [[nodiscard]] uint8_t GetCollisionLayer() const { return m_CollisionLayer; }
        [[nodiscard]] uint8_t GetDamageOnCollisionMask() const { return m_DamageOnCollisionMask; }
    
    private:
        bool m_IsStatic;
        SDL_Rect m_RectTransform;
        int m_MovementSpeed = 0;
        int m_RotationAngle = 0;
        uint8_t m_DamageOnCollisionMask = 0;
        uint8_t m_CollisionLayer = 0;
        Vector2D<int> m_Forward = Vector2D(0, -1);
    };
}