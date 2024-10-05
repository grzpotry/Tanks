#pragma once

#include <nlohmann/json.hpp>

#include "Vector2D.h"

class PhysicsComponent;
class Entity;

class EntityComponent
{
public:
	virtual ~EntityComponent()
	{
		//printf("Destroy component");
	}
	EntityComponent() = delete;
	EntityComponent(Entity* Owner);
	
	virtual std::unique_ptr<EntityComponent> Clone() const = 0;

	virtual void LoadFromConfig(nlohmann::json Config);
	virtual void Initialize();
	virtual void Update(float DeltaTime);
	virtual void Draw();
	virtual void UnInitialize();

	constexpr bool IsValid() const {return m_Owner != nullptr;}
	constexpr Entity* GetOwner() const { return m_Owner; }
	void SetOwner(Entity* Owner) { m_Owner = Owner; }


protected:
	//~EntityComponent() = default;

private:

	Entity* m_Owner;
};