#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include "Entity.h"
#include <nlohmann/json.hpp>
#include "Vector2D.h"
#include "Components/PhysicsComponent.h"

namespace Engine
{
    using namespace std;

    class Entity;

    class Scene
    {
        struct WeakPtrHash
        {
            size_t operator()(const weak_ptr<PhysicsComponent>& weakPtr) const
            {
                if (const auto sharedPtr = weakPtr.lock())
                {
                    return hash<PhysicsComponent*>()(sharedPtr.get());
                }
                return 0;
            }
        };

        // Comparator
        struct WeakPtrEqual
        {
            bool operator()(const weak_ptr<PhysicsComponent>& lhs, const weak_ptr<PhysicsComponent>& rhs) const
            {
                return lhs.lock().get() < rhs.lock().get();
            }
        };

    public:
        void SetPhysics(const shared_ptr<PhysicsComponent>& Physics, Vector2D<int> Position);

        void LoadFromConfig(nlohmann::json Config);
        int GetTileIndex(int Row, int Col) const;
        void Initialize();
        void UnInitialize();
        int QueryCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj);
        void UpdateCollisions();
        void UpdateDebugCollisions(float DeltaTime);
        void UpdateEntities(float DeltaTime);
        void Update(float DeltaTime);
        void Draw() const;
        void DrawDebugCollisions() const;

        void AddEntity(shared_ptr<Entity> Entity, Vector2D<int> Position);
        void AddEntity(shared_ptr<Entity> Entity);
        void AddProjectile(Vector2D<int> Position, Vector2D<int> Velocity, Entity* const Parent);

        bool TryFindBetterMovePosition(SDL_Rect SourceRect, const shared_ptr<PhysicsComponent>& SourceObj, SDL_Rect& FixedRect, bool AdjustX);

    private:
        int QueryStaticCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj = nullptr,bool bSilent = false);
        int QueryDynamicCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj = nullptr,bool bSilent = false);
        void LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend);

        bool bIsInitialized = false;
        int m_StaticTilesRows = 0;
        float m_CleanDebugAccumulator = 0.0;

        vector<shared_ptr<Entity>> m_Entities;
        vector<weak_ptr<PhysicsComponent>> m_StaticTiles; //TODO: cleanup expired tiles
        unordered_set<weak_ptr<PhysicsComponent>, WeakPtrHash, WeakPtrEqual> m_DebugCollisions;
        vector<weak_ptr<PhysicsComponent>> m_DynamicComponents;
        string m_Name;
    };
}
