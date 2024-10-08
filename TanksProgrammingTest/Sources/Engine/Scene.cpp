#include "Scene.h"
#include "Engine.h"
#include "ResourceManager.h"
#include <fstream>
#include "Vector2D.h"
#include "../Game/Components/PlayerComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/PhysicsComponent.h"

class ProjectileMovementComponent;

namespace EngineCore
{
	int Scene::GetTileIndex(const int Row, const int Col) const
	{
		return (Row * m_StaticTilesRows) + Col;
	}

	/*
	 * static tiles are laid out on grid so we can limit possible collision checks only to tiles in neighbourhood
	 */
	int Scene::QueryStaticCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj, bool bSilent)
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

				if (auto Target = m_StaticTiles.at(TileIndex).lock(); Target != nullptr && Target != SourceObj &&
					SDL_HasIntersection(&SourceRect, &Target->GetRectTransform()))
				{
					CollisionsCounter++;

					if (SourceObj && !bSilent)
					{
						SourceObj->OnCollision(Target);
						Target->OnCollision(SourceObj);
					}

					m_DebugCollisions.emplace(Target);
				}
			}
		}

		return CollisionsCounter;
	}

	/*
	 * Number of dynamic entities is expected to be low so brute force check should be sufficient for now.
	 * Can be optimized in future by introducing some spatial data structure eg. (Quadtree or Bounding Volume Hierarchies)
	 */
	int Scene::QueryDynamicCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj, bool bSilent)
	{
		int CollisionsCounter = 0;
	
		for (const auto& TargetPtr : m_DynamicComponents)
		{
			if (auto Target = TargetPtr.lock())
			{
				if (SourceObj == Target)
				{
					continue;
				}

				//TODO: optimize to skip some checks
				if (SDL_HasIntersection(&SourceRect, &Target->GetRectTransform()))
				{
					CollisionsCounter++;
			
					if (SourceObj && !bSilent)
					{
						SourceObj->OnCollision(Target);
						Target->OnCollision(SourceObj);
					}

					m_DebugCollisions.emplace(TargetPtr);
				}
			}
		}

		return CollisionsCounter;
	}


	int Scene::QueryCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const& SourceObj)
	{
		int CollisionsCounter = 0;

		CollisionsCounter += QueryDynamicCollisions(SourceRect, SourceObj, false);
		CollisionsCounter += QueryStaticCollisions(SourceRect, SourceObj, false);

		return CollisionsCounter;
	}

	void Scene::UpdateCollisions()
	{
		for (const auto& ObjAPtr : m_DynamicComponents)
		{
			if (auto Obj = ObjAPtr.lock())
			{
				QueryDynamicCollisions(Obj->GetRectTransform(), Obj, false);
				QueryStaticCollisions(Obj->GetRectTransform(), Obj, false);
			}
		}
	}

	void Scene::UpdateDebugCollisions(float DeltaTime)
	{
		m_CleanDebugAccumulator += DeltaTime;

		if (m_CleanDebugAccumulator > 1)
		{
			m_DebugCollisions.clear();
			m_CleanDebugAccumulator = 0;
		}
	}

	void Scene::Update(float DeltaTime)
	{
		UpdateEntities(DeltaTime);

#if DEBUG_COLLISIONS
		UpdateDebugCollisions(DeltaTime);
#endif
		UpdateCollisions();
		
		m_TimeToVictory -= DeltaTime;
	}

	void Scene::UpdateEntities(float DeltaTime)
	{
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

	void Scene::DrawDebugCollisions() const
	{
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
		//printf("Entities: %llu \n", m_Entities.size());
	}

	void Scene::AddProjectile(Vector2D<int> Position, Vector2D<int> Velocity, Entity* const Parent)
	{
		ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
		shared_ptr<Entity> NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate("Projectile", Parent);

		if (const auto Projectile = NewEntity->GetComponentWeak<ProjectileMovementComponent>().lock())
		{
			Projectile->SetVelocity(Velocity);
		}

		AddEntity(std::move(NewEntity), Position);
	}

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
                
				if (QueryCollisions(Potential, SourceObj) == 0)
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

	void Scene::LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend)
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
					nlohmann::json EntitySpecs = Legend[Key];
					shared_ptr<Entity> NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate(EntitySpecs["Type"]);
				
					if (auto Physics = NewEntity->GetComponentWeak<PhysicsComponent>().lock())
					{
						const SDL_Rect Rect = Physics->GetRectTransform();
						AddEntity(std::move(NewEntity), Vector2D(Column * Rect.w, Row * Rect.h));
					}
				}
				++Column;
			}
			++Row;
		}
	}
}
