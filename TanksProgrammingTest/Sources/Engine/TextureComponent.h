#pragma once

#include "EntityComponent.h"
#include "Engine.h"


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
		  m_TexturePath(other.m_TexturePath),
		  m_Rectangle(other.m_Rectangle)
	{
		if (m_TexturePath.length() > 0)
		{
			LoadTexture(m_TexturePath, m_TexturePtr);
		}
	}

	TextureComponent(TextureComponent&& other) noexcept: EntityComponent(other.GetOwner())
	{
		m_TexturePath = std::move(other.m_TexturePath);
		m_Rectangle = std::move(other.m_Rectangle);
		m_TexturePtr = std::move(other.m_TexturePtr);

		other.m_TexturePtr = nullptr;
	}

	TextureComponent& operator=(const TextureComponent& other)
	{
		if (this == &other)
			return *this;
		EntityComponent::operator =(other);
		m_TexturePath = other.m_TexturePath;
		m_Rectangle = other.m_Rectangle;
		
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
		m_Rectangle = std::move(other.m_Rectangle);
		m_TexturePtr = std::move(other.m_TexturePtr);
		
		other.m_TexturePtr = nullptr;
		return *this;
	}


	virtual EntityComponent* Clone() const override { return new TextureComponent(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	static void LoadTexture(std::string Path, std::unique_ptr<SDL_Texture, SdlDeleter>& Result);
	virtual void Initialize() override;
	virtual void UnInitialize() override;
	virtual void Draw() override;

	void SetTextureFromAssetName(std::string Name);
	void SetPosition(int x, int y);
	void SetScale(int w, int h);
	void SetRotationAngle(float angle);
	
	SDL_Rect& GetRectangle() { return m_Rectangle; }

protected:
	~TextureComponent()
	{
		m_TexturePtr = nullptr;
	}

private:
	
	std::string m_TexturePath;
	SDL_Rect m_Rectangle;
	SDL_Point m_Center;
	float m_RotationAngle = 0.0;
	std::unique_ptr<SDL_Texture, SdlDeleter> m_TexturePtr;
};