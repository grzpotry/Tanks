#pragma once

#include <set>
#include <vector>
#include <string>
#include "Entity.h"
#include <nlohmann/json.hpp>

#include "PhysicsComponent.h"

class Entity;

class Scene
{

public:
	void LoadFromConfig(nlohmann::json Config);
	int GetTileIndex(int Row, int Col) const;
	void Initialize();
	int QueryCollisions(SDL_Rect SourceRect, PhysicsComponent* const SourceObj);
	void Update(float DeltaTime);
	void DrawDebugCollisions();
	void Draw();
	void UnInitialize();

	void AddEntity(Entity* Entity);
	void RemoveEntity(Entity* Entity);
	std::set<PhysicsComponent*> m_DebugCollisions;

	float m_CleanDebugAccumulator;

private:
	int QueryStaticCollisions(SDL_Rect SourceRect, PhysicsComponent* const SourceObj = nullptr, bool bSilent = false);
	int QueryDynamicCollisions(SDL_Rect SourceRect, PhysicsComponent* const SourceObj = nullptr, bool bSilent = false);
	void LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend);
	
	std::list<std::unique_ptr<Entity>> m_Entities;
	std::vector<PhysicsComponent*> m_StaticTiles;
	int m_StaticTilesRows = 0;
	
	std::vector<PhysicsComponent*> m_DynamicComponents;
	std::string m_Name;
};