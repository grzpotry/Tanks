#pragma once

#include <nlohmann/json.hpp>

class Entity;

class EntityComponent
{
public:
	EntityComponent() = delete;
	EntityComponent(Entity* Owner);
	
	virtual EntityComponent* Clone() const = 0;

	virtual void LoadFromConfig(nlohmann::json Config);
	virtual void Initialize();
	virtual void Update(float DeltaTime);
	virtual void Draw();
	virtual void UnInitialize();

	bool IsValid() {return m_Owner != nullptr;}

	void SetOwner(Entity* Owner) { m_Owner = Owner; }
	Entity* GetOwner() { return m_Owner; }

protected:
	~EntityComponent() = default;

private:
	Entity* m_Owner;
};