#pragma once

#include <vector>
#include <nlohmann/json.hpp>

class EntityComponent;

class Entity
{
public:
	
	void LoadFromConfig(nlohmann::json Config);
	void Initialize();
	void Update(float DeltaTime);
	void Draw();
	void UnInitialize();
	void MarkDestroy();
	void Destroy();

	void AddComponent(std::unique_ptr<EntityComponent> Component);
	void RemoveComponent(EntityComponent* Component);

	template <typename ComponentType>
	std::weak_ptr<ComponentType> GetComponentWeak()
	{
		for (auto& Component : m_Components)
		{
			if (auto TypedComponent = std::dynamic_pointer_cast<ComponentType>(Component))
			{
				return TypedComponent;
			}
		}
		return std::weak_ptr<ComponentType>();
	}

	std::string GetName() {return m_Name;}

	bool IsPendingDestroy() {return bPendingDestroy;}
	
private:
	std::list<std::shared_ptr<EntityComponent>> m_Components;
	std::string m_Name;
	bool bPendingDestroy = false;
};