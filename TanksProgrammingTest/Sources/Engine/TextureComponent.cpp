#include "TextureComponent.h"
#include <filesystem>
#include "Engine.h"

TextureComponent::TextureComponent(Entity* Owner)
	: EntityComponent(Owner)
	, m_Rectangle{ 0,0,0,0 }
{
}

TextureComponent::TextureComponent()
	: TextureComponent(nullptr)
{
}

void TextureComponent::LoadFromConfig(nlohmann::json Config)
{
	std::string TextureName = Config.value("Texture", "");
	if (!TextureName.empty())
	{
		SetTextureFromAssetName(TextureName);
	}

	m_Rectangle.w = Config.value("Width", 10);
	m_Rectangle.h = Config.value("Height", 10);
	m_Rectangle.x = Config.value("PositionX", 0);
	m_Rectangle.y = Config.value("PositionY", 0);
}

void TextureComponent::LoadTexture(std::string Path, std::unique_ptr<SDL_Texture, SdlDeleter>& Result)
{
	SDL_Surface* Surface = IMG_Load(Path.c_str());

	if (!Surface)
	{
		printf("Couldn't load texture from path %s \n", Path.c_str());
		return;
	}
	
	Result.reset(SDL_CreateTextureFromSurface(Engine::Get()->GetRenderer(), Surface));
	SDL_FreeSurface(Surface);
}

void TextureComponent::Initialize()
{
	LoadTexture(m_TexturePath, m_TexturePtr);
}

void TextureComponent::UnInitialize()
{
	m_TexturePtr = nullptr;
}

void TextureComponent::Draw()
{
	if (m_TexturePtr != nullptr)
	{
		m_Center.x = m_Rectangle.w / 2;
		m_Center.y = m_Rectangle.h / 2;
		SDL_RenderCopyEx(Engine::Get()->GetRenderer(), m_TexturePtr.get(), nullptr, &m_Rectangle, m_RotationAngle, &m_Center, SDL_FLIP_NONE);
	}
}

void TextureComponent::SetTextureFromAssetName(std::string Name)
{
	m_TexturePath = "Resources/Images/" + Name;
}

void TextureComponent::SetPosition(int x, int y)
{
	m_Rectangle.x = x;
	m_Rectangle.y = y;
}

void TextureComponent::SetScale(int w, int h)
{
	m_Rectangle.w = w;
	m_Rectangle.h = h;
}

void TextureComponent::SetRotationAngle(float angle)
{
	m_RotationAngle = angle;
}
