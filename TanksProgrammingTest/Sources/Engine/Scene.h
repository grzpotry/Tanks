#pragma once

#include <set>
#include <vector>
#include <string>
#include <unordered_set>

#include "Entity.h"
#include <nlohmann/json.hpp>

#include "Vector2D.h"
#include "Components/PhysicsComponent.h"

class Entity;

class Scene
{

	struct WeakPtrHash {
		std::size_t operator()(const std::weak_ptr<PhysicsComponent>& weakPtr) const {
			// Get the shared_ptr from weak_ptr and hash the underlying raw pointer
			if (auto sharedPtr = weakPtr.lock()) {
				return std::hash<PhysicsComponent*>()(sharedPtr.get());
			}
			return 0; // Return 0 for expired weak_ptrs
		}
	};

	// Comparator for std::weak_ptr based on owner_before
	struct WeakPtrEqual {
		bool operator()(const std::weak_ptr<PhysicsComponent>& lhs, const std::weak_ptr<PhysicsComponent>& rhs) const {
			return lhs.lock().get() < rhs.lock().get();
		}
	};

public:
	void LoadFromConfig(nlohmann::json Config);
	int GetTileIndex(int Row, int Col) const;
	void Initialize();
	int QueryCollisions(SDL_Rect SourceRect, std::shared_ptr<PhysicsComponent> const SourceObj);
	void Update(float DeltaTime);
	void DrawDebugCollisions();
	void Draw();
	void UnInitialize();

	bool _flag = false;

	void AddEntity(std::unique_ptr<Entity> Entity);
	void AddProjectile(Vector2D<int> Position, Vector2D<int> Velocity);
	static void InitPhysics(std::shared_ptr<PhysicsComponent> Physics, int PositionX, int PositionY);
	std::unordered_set<std::weak_ptr<PhysicsComponent>,WeakPtrHash, WeakPtrEqual> m_DebugCollisions;

	float m_CleanDebugAccumulator;

private:
	int QueryStaticCollisions(SDL_Rect SourceRect, std::shared_ptr<PhysicsComponent> const& SourceObj = nullptr, bool bSilent = false);
	int QueryDynamicCollisions(SDL_Rect SourceRect, std::shared_ptr<PhysicsComponent> const SourceObj = nullptr, bool bSilent = false);
	void LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend);
	
	std::vector<std::unique_ptr<Entity>> m_Entities;
	std::vector<std::weak_ptr<PhysicsComponent>> m_StaticTiles; //TODO: cleanup expired tiles
	int m_StaticTilesRows = 0;
	
	std::vector<std::weak_ptr<PhysicsComponent>> m_DynamicComponents;
	std::string m_Name;
};