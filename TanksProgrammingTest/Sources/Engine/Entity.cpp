#include "Entity.h"
#include "Components/EntityComponent.h"
#include "Engine.h"
#include "ResourceManager.h"

void Entity::LoadFromConfig(nlohmann::json Config)
{
	m_Name = Config.value("Name", "");

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	
	if (Config.find("Components") != Config.end())
	{
		for (auto ComponentItem : Config["Components"].items())
		{
			nlohmann::json ComponentConfig = ComponentItem.value();
			std::string Type = ComponentConfig["Type"];
			const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName(Type);
			std::unique_ptr<EntityComponent> NewComponent = ComponentPrototype->Clone();
			
			NewComponent->SetOwner(this);
			NewComponent->LoadFromConfig(ComponentConfig);
			AddComponent(std::move(NewComponent));
		}
	}
}

void Entity::Initialize()
{
	for (const auto& Component : m_Components)
	{
		Component->Initialize();
	}
}

void Entity::Update(float DeltaTime)
{
	for (const auto& Component : m_Components)
	{
		Component->Update(DeltaTime);
	}
}

void Entity::Draw()
{
	for (const auto& Component : m_Components)
	{
		Component->Draw();
	}
}

void Entity::UnInitialize()
{
	for (const auto& Component : m_Components)
	{
		Component->UnInitialize();
	}
}

void Entity::Destroy()
{
	//printf("Destroy Entity\n");
	UnInitialize();
	m_Components.clear();
}

void Entity::MarkDestroy()
{
	bPendingDestroy = true;
	printf("Destroy Entity requested\n");
}

void Entity::AddComponent(std::unique_ptr<EntityComponent> Component)
{
	printf("AddComponent \n");
	m_Components.push_back(std::move(Component));
}

void Entity::RemoveComponent(EntityComponent* Component)
{
	//m_Components.remove(Component);
}
