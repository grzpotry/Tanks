#pragma once

#include "EntityComponent.h"
#include "Engine.h"
#include "PhysicsComponent.h"


struct SdlDeleter
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

	TextureComponent(const TextureComponent& other)
		: EntityComponent(other),
		  m_TexturePath(other.m_TexturePath)
	{
		if (m_TexturePath.length() > 0)
		{
			LoadTexture(m_TexturePath, m_TexturePtr);
		}
	}

	TextureComponent(TextureComponent&& other) noexcept: EntityComponent(other.GetOwner())
	{
		m_TexturePath = std::move(other.m_TexturePath);
		m_TexturePtr = std::move(other.m_TexturePtr);

		other.m_TexturePtr = nullptr;
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


	virtual EntityComponent* Clone() const override { return new TextureComponent(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	static void LoadTexture(std::string Path, std::unique_ptr<SDL_Texture, SdlDeleter>& OutResult);
	virtual void Initialize() override;
	virtual void UnInitialize() override;
	virtual void Draw() override;

	void SetTextureFromAssetName(std::string Name);

protected:
	~TextureComponent()
	{
		m_TexturePtr = nullptr;
	}

private:
	
	std::string m_TexturePath;
	PhysicsComponent* m_PhysicsComponent;
	std::unique_ptr<SDL_Texture, SdlDeleter> m_TexturePtr;
	
};