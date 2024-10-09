#include "Scene.h"
#include "Engine.h"
#include "ResourceManager.h"
#include <fstream>

#include "CollisionUtils.h"
#include "EngineUtils.h"
#include "Vector2D.h"
#include "../Game/Components/PlayerComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/TeamComponent.h"

class ProjectileMovementComponent;

namespace EngineCore
{
	enum PositionMarker : uint8_t
	{
		None = 0,
		EnemySpawn = 1 << 0,
		PlayerSpawn = 1 << 1,
	};

	static const std::unordered_map<string, PositionMarker> PositionMarkerNameToEnum =
	{
		{"EnemySpawn", PositionMarker::EnemySpawn},
		{"PlayerSpawn", PositionMarker::PlayerSpawn},
	};
	
	int Scene::GetTileIndex(const int Row, const int Col) const
	{
		return (Row * m_StaticTilesRows) + Col;
	}

	bool Scene::HasIntersection(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> Target)
	{
		//EngineUtils::ProfileScope _("HasIntersection", false);
		auto result = SDL_HasIntersection(&SourceRect, &Target->GetRectTransform());
		return result;
	}

	/*
	 * static tiles are laid out on grid so we can limit possible collision checks only to tiles in neighbourhood
	 */
	int Scene::QueryStaticCollisions(SDL_Rect SourceRect, CollisionLayer CollisionMask, shared_ptr<PhysicsComponent> const& SourceObj, bool bStopOnFirstCollision, bool bSilent) const
	{
		//TODO: hardcoded size of static tile, add support for more sizes based on entities data
		constexpr int CellWidth = 30;
		constexpr int CellHeigh = 35;

		const int StartCellX = SourceRect.x / CellWidth;
		const int StartCellY = SourceRect.y / CellHeigh;

		// more greedy check on end since pivot is in upper-left corner
		const int EndCellX = (SourceRect.x + SourceRect.w) / CellWidth + 1;
		const int EndCellY = (SourceRect.y + SourceRect.h) / CellHeigh + 1;

		int CollisionsCounter = 0;
	
		for (int i = StartCellX; i < EndCellX; i++)
		{
			for (int j = StartCellY; j < EndCellY; j++)
			{
				const int TileIndex = GetTileIndex(i, j);

				if (TileIndex >= static_cast<int>(m_StaticTiles.size()) || TileIndex < 0)
				{
					continue;
				}

				if (auto Target = m_StaticTiles.at(TileIndex).lock(); Target != nullptr
					&& Target != SourceObj // skip self
					&& (static_cast<uint8_t>(CollisionMask) & Target->GetCollisionLayer()) // filter out object from not matching layers
					&& HasIntersection(SourceRect, Target))
				{
					CollisionsCounter++;
					
					if (SourceObj && !bSilent)
					{
						SourceObj->OnCollision(Target);
						Target->OnCollision(SourceObj);
					}

#if DEBUG_COLLISIONS
					m_DebugCollisions.emplace(Target);
#endif

					if (bStopOnFirstCollision)
					{
						return CollisionsCounter;
					}
				}
			}
		}

		return CollisionsCounter;
	}

	/*
	 * Number of dynamic entities is expected to be low so brute force check should be sufficient for now.
	 * Can be optimized in future by introducing some spatial data structure eg. (Quadtree or Bounding Volume Hierarchies)
	 */
	int Scene::QueryDynamicCollisions(SDL_Rect SourceRect, CollisionLayer CollisionMask, shared_ptr<PhysicsComponent> const& SourceObj, bool bStopOnFirstCollision, bool bSilent)
	{
		int CollisionsCounter = 0;

		for (auto it = m_DynamicComponents.begin(); it != m_DynamicComponents.end();)
		{
			//clear destroyed objects
			if (it->expired())
			{
				it = m_DynamicComponents.erase(it);
			}
			else
			{
				if (auto Target = it->lock())
				{
					if (SourceObj == Target || !(static_cast<uint8_t>(CollisionMask) & Target->GetCollisionLayer()))
					{
						++it;
						continue;
					}

					// // fast skip - remove? not really that faster than rect 
					// if (abs(Target->GetBoundingTile().X - SourceObj->GetBoundingTile().X) > 1 || abs(Target->GetBoundingTile().Y - SourceObj->GetBoundingTile().Y) > 1)
					// {
					// 	++it;
					// 	continue;
					// }
					
					if (SDL_HasIntersection(&SourceRect, &Target->GetRectTransform()))
					{
						CollisionsCounter++;

						if (SourceObj && !bSilent)
						{
							SourceObj->OnCollision(Target);
							Target->OnCollision(SourceObj);
						}

#if DEBUG_COLLISIONS
						m_DebugCollisions.emplace(Target);
#endif

						if (bStopOnFirstCollision)
						{
							return CollisionsCounter;
						}
					}
				}

				++it;
			}
		}

		return CollisionsCounter;
	}


	int Scene::QueryCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj, bool bSilent, bool bStopOnFirstCollision, CollisionLayer
	                           CollisionMask)
	{
		int CollisionsCounter = 0;

		CollisionsCounter += QueryDynamicCollisions(SourceRect, CollisionMask, SourceObj, bStopOnFirstCollision, bSilent);
		CollisionsCounter += QueryStaticCollisions(SourceRect, CollisionMask, SourceObj, bStopOnFirstCollision, bSilent);

		return CollisionsCounter;
	}

	void Scene::UpdateCollisions()
	{
		for (const auto& ObjAPtr : m_DynamicComponents)
		{
			if (auto Obj = ObjAPtr.lock())
			{
				QueryDynamicCollisions(Obj->GetRectTransform(), CollisionLayer::All, Obj, false, false);
				QueryStaticCollisions(Obj->GetRectTransform(), CollisionLayer::All, Obj, false, false);
			}
		}
	}

	void Scene::UpdateDebugCollisions(float DeltaTime)
	{
#if DEBUG_COLLISIONS
		m_CleanDebugAccumulator += DeltaTime;

		if (m_CleanDebugAccumulator > 1)
		{
			m_DebugCollisions.clear();
			m_CleanDebugAccumulator = 0;
		}
#endif
	}

	void Scene::Update(float DeltaTime)
	{
		EngineUtils::ProfileScope _("Scene::Update");

		UpdateEntities(DeltaTime);

		UpdateEnemies(DeltaTime);
		
		UpdateDebugCollisions(DeltaTime);

		UpdateCollisions();
		
		m_TimeToVictory -= DeltaTime;
	}

	void Scene::UpdateEntities(float DeltaTime)
	{
		EngineUtils::ProfileScope _("UpdateEntities");
		
		for (auto it = m_Entities.begin(); it != m_Entities.end(); )
		{
			if ((*it)->IsPendingDestroy())
			{
				(*it)->Destroy();
				it = m_Entities.erase(it);
				//printf("Entities: %llu \n", m_Entities.size());
				continue;
			}

			(*it)->Update(DeltaTime);
			++it;
		}
	}

	void Scene::UpdateEnemies(float DeltaTime)
	{
		m_EnemiesSpawnTimer += DeltaTime;

		if (m_EnemiesSpawnTimer < 1)
		{
			return;
		}

		m_EnemiesSpawnTimer = 0;
		
		for (auto it = m_Enemies.begin(); it != m_Enemies.end();)
		{
			//clear destroyed enemies
			if (it->expired())
			{
				it = m_Enemies.erase(it);
				printf("Removed enemy \n");
				continue;
			}

			++it;
		}

		if (m_Enemies.size() < m_EnemySpawnPositions.size())
		{
			AddPredefinedEntity(m_EnemySpawnPositions, m_Enemies, "Enemy");
		}
	}

	void Scene::DrawDebugCollisions()
	{
#if DEBUG_COLLISIONS
		for (auto& ElementPtr : m_DebugCollisions)
		{
			if (const auto Element = ElementPtr.lock())
			{
				auto rect_transform = Element->GetRectTransform();
				Uint8 prevR, prevG, prevB, prevA;
		
				SDL_GetRenderDrawColor(Engine::Get()->GetRenderer(), &prevR, &prevG, &prevB, &prevA);
				SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), 255, 00, 00, 255);
				SDL_RenderFillRect(Engine::Get()->GetRenderer(), &rect_transform);
				SDL_SetRenderDrawColor(Engine::Get()->GetRenderer(), prevR, prevG, prevB, prevA);
			}
		}
#endif
	}

	void Scene::Draw() const
	{
		for (const auto& Entity : m_Entities)
		{
			Entity->Draw();
		}

#if DEBUG_COLLISIONS
		DrawDebugCollisions();
#endif
	}

	void Scene::Initialize(GameModeBase* const ActiveGame)
	{
		if (bIsInitialized)
		{
			return;
		}

		m_ActiveGame = ActiveGame;
		
		for (const auto& Entity : m_Entities)
		{
			Entity->Initialize(ActiveGame);
		}
		
		bIsInitialized = true;
	}

	void Scene::UnInitialize()
	{
		for (const auto& Entity : m_Entities)
		{
			Entity->UnInitialize();
		}
	
		m_Entities.clear();
		bIsInitialized = false;
	}

	void Scene::AddEntity(shared_ptr<Entity> Entity, const Vector2D<int> Position)
	{
		if (const auto Physics = Entity->GetComponentWeak<PhysicsComponent>().lock())
		{
			SetPhysics(Physics, Position);
		}
		
		if (bIsInitialized)
		{
			Entity->Initialize(m_ActiveGame);
		}
		
		AddEntity(std::move(Entity));
	}

	void Scene::AddEntity(shared_ptr<Entity> Entity)
	{
		m_Entities.push_back(std::move(Entity));
	}

	void Scene::AddProjectile(Vector2D<int> Position, Vector2D<int> Velocity, Entity* const Parent)
	{
		ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
		shared_ptr<Entity> NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate("Projectile", Parent);

		if (const auto Projectile = NewEntity->GetComponentWeak<ProjectileMovementComponent>().lock())
		{
			Projectile->SetVelocity(Velocity);
		}

		if (const auto SelfTeam = NewEntity->GetComponentWeak<TeamComponent>().lock())
		{
			if (const auto ParentTeam = Parent->GetComponentWeak<TeamComponent>().lock())
			{
				SelfTeam->SetTeamId(ParentTeam->GetTeamId());
			}
		}

		AddEntity(std::move(NewEntity), Position);
	}

	shared_ptr<Entity> Scene::AddPlayer()
	{
		shared_ptr<Entity> PlayerEntity;
		CreateEntity("Player",PlayerEntity);
		AddEntity(PlayerEntity, GetRandomPosition(m_PlayerSpawnPositions));
		return PlayerEntity;
	}

	void Scene::CreateEntity(const string& TemplateName, shared_ptr<Entity>& NewEntity)
	{
		ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
		NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate(TemplateName);
	}

	Vector2D<int> Scene::GetRandomPosition(const vector<Vector2D<int>>& PossiblePositions)
	{
		std::uniform_int_distribution<std::size_t> dist(0, PossiblePositions.size() - 1);
		return PossiblePositions[dist(*Engine::Get()->GetRandomGenerator())];
	}

	void Scene::AddPredefinedEntity(const vector<Vector2D<int>>& PossiblePositions, list<weak_ptr<Entity>>& Container, const string& TemplateName)
	{
		if (PossiblePositions.empty())
		{
			return;
		}
		shared_ptr<Entity> NewEntity;
		CreateEntity(TemplateName, NewEntity);

		const Vector2D<int> position = GetRandomPosition(PossiblePositions);

		if (auto b = NewEntity->GetComponentWeak<PhysicsComponent>().lock())
		{
			SDL_Rect SpawnRect = b->GetRectTransform();
			SpawnRect.x = position.X;
			SpawnRect.y = position.Y;

			if (QueryCollisions(SpawnRect, b, true, true, CollisionLayer::All) == 0)
			{
				Container.emplace_back(NewEntity);
				AddEntity(std::move(NewEntity), position);
			}
		}
		
	}

	//TODO: skip projectiles
	bool Scene::TryFindBetterMovePosition(SDL_Rect SourceRect, const shared_ptr<PhysicsComponent>& SourceObj,
		SDL_Rect& FixedRect, bool AdjustX)
	{
		const int MaxFix = (AdjustX ? SourceRect.h : SourceRect.w) / 2;

		for (int i = 0; i < MaxFix; i++)
		{
			for (int dir = -1; dir <= 1; dir += 2)
			{
				SDL_Rect Potential = SourceRect;
                
				if (AdjustX)
				{
					Potential.x += dir * i;
				}
				else
				{
					Potential.y += dir * i;
				}
                
				if (QueryCollisions(Potential, SourceObj, true, true, CollisionLayer::WallsAndTanks) == 0)
				{
					FixedRect = Potential;
					return true;
				}
			}
		}

		return false;
	}
	void Scene::SetPhysics(const shared_ptr<PhysicsComponent>& Physics, const Vector2D<int> Position)
	{
		Physics->SetPosition(Position.X, Position.Y);

		const SDL_Rect Rect = Physics->GetRectTransform();

		if (Physics->IsStatic())
		{
			const int Row = Rect.x / Rect.w;
			const int Col = Rect.y / Rect.h;
			const int Index = GetTileIndex(Row, Col);

			if (Index >= static_cast<int>(m_StaticTiles.size()))
			{
				m_StaticTiles.resize(Index + 1);
			}
					
			m_StaticTiles[Index] = Physics;
		}
		else
		{
			m_DynamicComponents.push_back(Physics);
		}

		Physics->SetScale(Rect.w, Rect.h);
	}

	void Scene::LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend, nlohmann::json PositionMarkers)
	{
		printf("LoadSceneFromLayout \n");
	
		int Row = 0;
		ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	
		m_StaticTilesRows = 0;
		int MaxCols = 0;
	
		for (auto Item : Content.items())
		{
			const string& Line = Item.value();
			MaxCols = max(MaxCols, static_cast<int>(Line.length()));
			m_StaticTilesRows++;
		}

		int Capacity = m_StaticTilesRows * MaxCols;
		m_StaticTiles.resize(Capacity);
	
		for (auto Item : Content.items())
		{
			int Column = 0;
			const string& Line = Item.value();
			for (char Character : Line)
			{
				if (Character != ' ')
				{
					const char Key[] = { Character, '\0' };
					
					if (!Legend.contains(Key))
					{
						if (PositionMarkers.contains(Key))
						{
							const string& marker = PositionMarkers[Key]["Type"];

							auto it = PositionMarkerNameToEnum.find(marker);

							if (const bool bIsMarkerFound = it != PositionMarkerNameToEnum.end())
							{
								switch (it->second)
								{
								case EnemySpawn:
									printf("registered spawn position \n");
									m_EnemySpawnPositions.emplace_back(Column * 30, Row * 35);
									break;
								case PlayerSpawn:
									printf("registered spawn position \n");
									m_PlayerSpawnPositions.emplace_back(Column * 30, Row * 35);
									break;
								case None:
									printf("");
								}
							}
						}
						else
						{
							printf("invalid marker");
						}
					}
					else
					{
						nlohmann::json EntitySpecs = Legend[Key];
						shared_ptr<Entity> NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate(EntitySpecs["Type"]);
				
						if (auto Physics = NewEntity->GetComponentWeak<PhysicsComponent>().lock())
						{
							const SDL_Rect Rect = Physics->GetRectTransform();
							AddEntity(std::move(NewEntity), Vector2D(Column * Rect.w, Row * Rect.h));
						}
					}
				}
				++Column;
			}
			++Row;
		}
	}
}
