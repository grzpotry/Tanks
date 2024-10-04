#pragma once

#include "Components/EntityComponent.h"

class PhysicsComponent;

class PlayerInputComponent : public EntityComponent
{

public:
	PlayerInputComponent(Entity* Owner);
	PlayerInputComponent();

	virtual EntityComponent* Clone() const override { return new PlayerInputComponent(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

private:
	PhysicsComponent* m_PhysicsComponent;
};