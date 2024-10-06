#include "Scene.h"
#include "Engine.h"
#include "ResourceManager.h"
#include <fstream>
#include "Vector2D.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/PhysicsComponent.h"

namespace Engine
{
	class ProjectileMovementComponent;

	void Scene::LoadFromConfig(nlohmann::json Config)
	{
		printf("LoadSceneFromConfig \n");
		m_Name = Config.value("Name", "");

		if (Config.find("Entities") != Config.end())
		{
			ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
			for (auto Item : Config["Entities"].items())
			{
				auto NewEntity = make_unique<Entity>();

				nlohmann::json EntityConfig = Item.value();
				string TypeName = EntityConfig.value("Type", "");
				if (!TypeName.empty())
				{
					nlohmann::json EntityTemplateConfig = ResourceManagerPtr->GetJsonConfig(TypeName, ResourceType::Entity);
					NewEntity->LoadFromConfig(EntityTemplateConfig);
				}
				else
				{
					NewEntity->LoadFromConfig(Item.value());
				}

				AddEntity(move(NewEntity));
			}
		}

		if (const nlohmann::json::const_iterator SceneLayoutIt = Config.find("SceneLayout"); SceneLayoutIt != Config.end())
		{
			LoadSceneFromLayout((*SceneLayoutIt)["Content"], (*SceneLayoutIt)["Legend"]);
		}
	}

	int Scene::GetTileIndex(const int Row, const int Col) const
	{
		return (Row * m_StaticTilesRows) + Col;
	}

	void Scene::Initialize()
	{
		for (const auto& Entity : m_Entities)
		{
			Entity->Initialize();
		}

		for (const auto& Entity : m_Entities)
		{
			if (auto Physics = Entity->GetComponentWeak<PhysicsComponent>().lock())
			{
				if (Physics->IsStatic())
				{
					auto Rect = Physics->GetRectTransform();
					const int Row = Rect.x / Rect.w;
					const int Col = Rect.y / Rect.h;
					const int index = GetTileIndex(Row, Col);

					if (index >= static_cast<int>(m_StaticTiles.size())) {
						m_StaticTiles.resize(index + 1);
					}

					//assert (m_StaticTiles[index] == nullptr);

					m_StaticTiles[index] = Physics;
					//printf("Add tile %i %s size %i %i pos %i %i \n", index, Entity->GetName().c_str(), Rect.w, Rect.h, Rect.x, Rect.y);
				}
				else
				{
					m_DynamicComponents.push_back(Physics);
				}
			}
		}
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


	int Scene::QueryCollisions(SDL_Rect SourceRect, shared_ptr<PhysicsComponent> const SourceObj)
	{
		int CollisionsCounter = 0;

		CollisionsCounter += QueryDynamicCollisions(SourceRect, SourceObj, false);
		CollisionsCounter += QueryStaticCollisions(SourceRect, SourceObj, false);

		return CollisionsCounter;
	}

	void Scene::Update(float DeltaTime)
	{
		for (auto it = m_Entities.begin(); it != m_Entities.end(); )
		{
			if ((*it)->IsPendingDestroy())
			{
				(*it)->Destroy();
				it = m_Entities.erase(it);
				printf("Entities: %i \n", m_Entities.size());
				continue;
			}

			(*it)->Update(DeltaTime);
			++it;
		}
	
		m_CleanDebugAccumulator += DeltaTime;

		if (m_CleanDebugAccumulator > 1)
		{
			m_DebugCollisions.clear();
			m_CleanDebugAccumulator = 0;
		}
	
		for (const auto& ObjAPtr : m_DynamicComponents)
		{
			if (auto Obj = ObjAPtr.lock())
			{
				QueryDynamicCollisions(Obj->GetRectTransform(), Obj, false);
				QueryStaticCollisions(Obj->GetRectTransform(), Obj, false);
			}
		}
	}

	void Scene::DrawDebugCollisions()
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

	void Scene::Draw()
	{
		for (const auto& Entity : m_Entities)
		{
			Entity->Draw();
		}

#if DEBUG_COLLISIONS
		DrawDebugCollisions();
#endif
	}

	void Scene::UnInitialize()
	{
		for (const auto& Entity : m_Entities)
		{
			Entity->UnInitialize();
		}
	
		m_Entities.clear();
	}

	void Scene::AddEntity(unique_ptr<Entity> Entity)
	{
		m_Entities.push_back(move(Entity));

		printf("Entities: %i \n", m_Entities.size());
	}

	void Scene::AddProjectile(Vector2D<int> Position, Vector2D<int> Velocity)
	{
		ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
		unique_ptr<Entity> NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate("Projectile");

		if (auto Physics = NewEntity->GetComponentWeak<PhysicsComponent>().lock())
		{
			InitPhysics(Physics, Position.X, Position.Y);
			m_DynamicComponents.push_back(Physics); //todo: merge to single place
		}

		if (const auto Projectile = NewEntity->GetComponentWeak<ProjectileMovementComponent>().lock())
		{
			Projectile->SetVelocity(Velocity);
		}

		NewEntity->Initialize();
		AddEntity(move(NewEntity));
	}

	void Scene::InitPhysics(shared_ptr<PhysicsComponent> Physics, int PositionX, int PositionY)
	{
		const SDL_Rect Rect = Physics->GetRectTransform();
		Physics->SetPosition(PositionX, PositionY);
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
		printf("Reserved %i", Capacity);
	
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
					unique_ptr<Entity> NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate(EntitySpecs["Type"]);
				
					if (auto Physics = NewEntity->GetComponentWeak<PhysicsComponent>().lock())
					{
						const SDL_Rect Rect = Physics->GetRectTransform();
				
						InitPhysics(Physics, Column * Rect.w, Row * Rect.h);
						AddEntity(std::move(NewEntity));
					}
			
				}
				++Column;
			}
			++Row;
		}
	}
}
