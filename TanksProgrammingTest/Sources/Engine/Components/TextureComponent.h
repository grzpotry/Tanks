#pragma once

#include "EntityComponent.h"
#include "Engine.h"
#include "PhysicsComponent.h"

struct SDL_Deleter
{
	void operator()(SDL_Window* p) const { SDL_DestroyWindow(p); }
	void operator()(SDL_Renderer* p) const { SDL_DestroyRenderer(p); }

	void operator()(SDL_Texture* p) const
	{
		SDL_DestroyTexture(p);
	}
};

class TextureComponent : public EntityComponent
{
public:
	TextureComponent(Entity* Owner);
	TextureComponent();

	~TextureComponent()
	{
		m_TexturePtr = nullptr;
	}

	TextureComponent(const TextureComponent& other)
		: EntityComponent(other),
		  m_TexturePath(other.m_TexturePath)
	{
		if (m_TexturePath.length() > 0)
		{
			LoadTexture(m_TexturePath, m_TexturePtr);
		}
	}

	TextureComponent(TextureComponent&& other) noexcept:
		EntityComponent(static_cast<EntityComponent&>(other)),
		m_TexturePath(std::move(other.m_TexturePath)),
		m_TexturePtr(std::move(other.m_TexturePtr))
	{
	}

	TextureComponent& operator=(const TextureComponent& other)
	{
		if (this == &other)
			return *this;
		
		EntityComponent::operator =(other);
		m_TexturePath = other.m_TexturePath;
		
		if (m_TexturePath.length() > 0)
		{
			LoadTexture(m_TexturePath, m_TexturePtr);
		}

		return *this;
	}

	TextureComponent& operator=(TextureComponent&& other) noexcept
	{
		if (this == &other)
			return *this;
		
		m_TexturePath = std::move(other.m_TexturePath);
		m_TexturePtr = std::move(other.m_TexturePtr);
		
		other.m_TexturePtr = nullptr;
		return *this;
	}
	
	std::unique_ptr<EntityComponent> Clone() const override { return std::make_unique<TextureComponent>(*this); }
	void LoadFromConfig(nlohmann::json Config) override;
	void Initialize() override;
	void UnInitialize() override;
	void Draw() override;

	void SetTextureFromAssetName(std::string Name);
	
	static void LoadTexture(std::string Path, std::unique_ptr<SDL_Texture, SDL_Deleter>& OutResult);

protected:

private:
	std::weak_ptr<PhysicsComponent> m_PhysicsComponent;
	std::string m_TexturePath;
	std::unique_ptr<SDL_Texture, SDL_Deleter> m_TexturePtr;
	
};