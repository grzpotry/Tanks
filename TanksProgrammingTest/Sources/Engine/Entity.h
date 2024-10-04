#pragma once

#include <vector>
#include <nlohmann/json.hpp>

class EntityComponent;

class Entity
{
public:

	~Entity()
	{
	}
	
	void LoadFromConfig(nlohmann::json Config);
	void Initialize();
	void Update(float DeltaTime);
	void Draw();
	void UnInitialize();
	void Destroy();

	void AddComponent(EntityComponent* Component);
	void RemoveComponent(EntityComponent* Component);

	template <typename ComponentType>
	ComponentType* GetComponent()
	{
		for (EntityComponent* Component : m_Components)
		{
			if (ComponentType* TypedComponent = dynamic_cast<ComponentType*>(Component))
			{
				return TypedComponent;
			}
		}
		return nullptr;
	}

	std::string GetName() {return m_Name;}

	bool IsMarkedToDestroy() {return bDestroy;}
	
private:
	std::list<EntityComponent*> m_Components;
	std::string m_Name;
	bool bDestroy = false;
};