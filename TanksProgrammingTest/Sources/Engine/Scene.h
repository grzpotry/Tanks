#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include "Entity.h"
#include <nlohmann/json.hpp>
#include "CollisionUtils.h"
#include "Vector2D.h"
#include "Components/PhysicsComponent.h"

namespace EngineCore
{
    using namespace std;

    class Entity;

    // Game scene holding all entities, handles collisions
    // TODO: too big, refactor needed; game specific code should be extracted and moved, collisions management should be also extracted
    class Scene
    {
    public:
        explicit Scene(const string& m_name, float TimeToVictory, float EnemyRespawnCooldown, int MinEnemies)
            : m_Name(m_name), m_TimeToVictory(TimeToVictory), m_EnemyRespawnCooldown(EnemyRespawnCooldown), m_MinEnemies(MinEnemies)
        {
        }

        ~Scene()
        {
            printf("Destroyed Scene \n");
        }
        
        static bool HasIntersection(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> Target);
        static Vector2D<int> GetRandomPosition(const vector<Vector2D<int>>& PossiblePositions);
        
        static void DrawDebugCollisions();
        int QueryCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj, bool bSilent, bool bStopOnFirstCollision, CollisionLayer CollisionMask);
        void UpdateCollisions();
        void UpdateDebugCollisions(float DeltaTime);
        bool TryFindBetterMovePosition(SDL_Rect SourceRect, const shared_ptr<PhysicsComponent>& SourceObj, SDL_Rect& FixedRect, bool AdjustX);
        
        void SetPhysics(const shared_ptr<PhysicsComponent>& Physics, Vector2D<int> Position);
        int GetTileIndex(int Row, int Col) const;
        
        void Initialize(GameModeBase* const ActiveGame);
        void UnInitialize();
        
        void TrySpawnSingleEnemy(list<weak_ptr<Entity>>& Enemies, vector<Vector2D<int>>& SpawnPositions, const string& TemplateName, bool& IsEnemyAlreadySpawned);
        void SpawnEnemies(float DeltaTime);
        
        void Update(float DeltaTime);
        void Draw() const;

        void AddEntity(shared_ptr<Entity> Entity, Vector2D<int> Position);
        void RemoveEntity(const shared_ptr<Entity>& Entity);
        void UpdateEntities(float DeltaTime);
        
        void AddProjectile(Vector2D<int> Position, Vector2D<int> Direction, Entity* const Parent);
        void AddPlayer(const shared_ptr<Entity>& Player);
        void AddPredefinedEntity(const vector<Vector2D<int>>& PossiblePositions, list<weak_ptr<Entity>>& Container, const string& TemplateName);
        
        void LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend, nlohmann::json PositionMarkers);
        
        float GetTimeToVictory() const { return m_TimeToVictory; }

    private:
        void AddEntity(shared_ptr<Entity> Entity);
        int QueryStaticCollisions(SDL_Rect SourceRect, CollisionLayer CollisionMask, shared_ptr<PhysicsComponent> const& SourceObj = nullptr, bool bStopOnFirstCollision = false, bool bSilent = false) const;
        int QueryDynamicCollisions(SDL_Rect SourceRect, CollisionLayer CollisionMask, shared_ptr<PhysicsComponent> const& SourceObj = nullptr, bool bStopOnFirstCollision = false, bool bSilent = false);

        float m_EnemiesSpawnTimer = 0.0f;
        float m_TimeToVictory = 0; 
        float m_EnemyRespawnCooldown = 0; 
        int m_MinEnemies = 0; 
        bool m_IsInitialized = false;
        int m_StaticTilesRows = 0;
        float m_CleanDebugAccumulator = 0.0;
        string m_Name;
        
        GameModeBase* m_ActiveGame = nullptr;
        list<shared_ptr<Entity>> m_Entities;
        
        vector<weak_ptr<PhysicsComponent>> m_StaticTiles;
        list<weak_ptr<PhysicsComponent>> m_DynamicComponents;

        //TODO: could be stored in single map/struct
        vector<Vector2D<int>> m_EnemySpawnPositions;
        vector<Vector2D<int>> m_HeavyEnemySpawnPositions;
        vector<Vector2D<int>> m_FastEnemySpawnPositions;
        vector<Vector2D<int>> m_PlayerSpawnPositions;

        list<weak_ptr<Entity>> m_Enemies;
        list<weak_ptr<Entity>> m_HeavyEnemies;
        list<weak_ptr<Entity>> m_FastEnemies;

#if DEBUG_COLLISIONS
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
        
        unordered_set<weak_ptr<PhysicsComponent>, WeakPtrHash, WeakPtrEqual> m_DebugCollisions;
#endif
    };
}
