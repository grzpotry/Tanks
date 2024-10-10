#pragma once

#include "EntityComponent.h"
#include "Engine.h"
#include "PhysicsComponent.h"

namespace EngineCore
{
	using namespace std;

	// draws texture on screen
	class TextureComponent : public EntityComponent
	{
	public:
		TextureComponent(Entity* Owner);
		TextureComponent();

		TextureComponent(const TextureComponent& other)
			: EntityComponent(other),
			  m_TexturePath(other.m_TexturePath),
			  m_TexturePtr(nullptr)
		{
		}

		TextureComponent(TextureComponent&& other) noexcept:
			EntityComponent(static_cast<EntityComponent&>(other)),
			m_TexturePath(move(other.m_TexturePath)),
			m_TexturePtr(move(other.m_TexturePtr))
		{
		}

		TextureComponent& operator=(const TextureComponent& other)
		{
			if (this == &other)
				return *this;
		
			EntityComponent::operator =(other);
			m_TexturePath = other.m_TexturePath;
			m_TexturePtr = other.m_TexturePtr;

			return *this;
		}

		TextureComponent& operator=(TextureComponent&& other) noexcept
		{
			if (this == &other)
				return *this;
		
			m_TexturePath = move(other.m_TexturePath);
			m_TexturePtr = move(other.m_TexturePtr);
		
			other.m_TexturePtr = nullptr;
			return *this;
		}

		[[nodiscard]] unique_ptr<EntityComponent> Clone() const override { return make_unique<TextureComponent>(*this); }
		void LoadFromConfig(nlohmann::json Config) override;
		void Initialize(GameModeBase* Game) override;
		void UnInitialize() override;
		void Draw() override;
		void SetTextureFromAssetName(string Name);

	private:
		weak_ptr<PhysicsComponent> m_PhysicsComponentWeak;
		shared_ptr<SDL_Texture> m_TexturePtr;
		string m_TexturePath;
	};
}