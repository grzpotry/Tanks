#pragma once

#include "Components/EntityComponent.h"

class PhysicsComponent;

class PlayerInputComponent : public EntityComponent
{

public:
	PlayerInputComponent(Entity* Owner);
	PlayerInputComponent();
	
	std::unique_ptr<EntityComponent> Clone() const override { return std::make_unique<PlayerInputComponent>(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

private:
	std::weak_ptr<PhysicsComponent> m_PhysicsComponent;
};