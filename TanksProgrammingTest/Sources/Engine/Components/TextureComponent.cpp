#include "TextureComponent.h"
#include <filesystem>
#include "Engine.h"
#include "Entity.h"

class PhysicsComponent;

TextureComponent::TextureComponent(Entity* Owner)
	: EntityComponent(Owner), m_PhysicsComponent(nullptr)
{
}

TextureComponent::TextureComponent()
	: TextureComponent(nullptr)
{
}

void TextureComponent::LoadFromConfig(nlohmann::json Config)
{
	const std::string TextureName = Config.value("Texture", "");
	
	if (!TextureName.empty())
	{
		SetTextureFromAssetName(TextureName);
	}
}

void TextureComponent::LoadTexture(std::string Path, std::unique_ptr<SDL_Texture, SDL_Deleter>& OutResult)
{
	SDL_Surface* Surface = IMG_Load(Path.c_str());

	if (!Surface)
	{
		printf("Couldn't load texture from path %s \n", Path.c_str());
		return;
	}
	
	OutResult.reset(SDL_CreateTextureFromSurface(Engine::Get()->GetRenderer(), Surface));
	SDL_FreeSurface(Surface);
}

void TextureComponent::Initialize()
{
	m_PhysicsComponent = GetOwner()->GetComponent<PhysicsComponent>();
	LoadTexture(m_TexturePath, m_TexturePtr);
}

void TextureComponent::UnInitialize()
{
	m_TexturePtr = nullptr;
}

void TextureComponent::Draw()
{
	if (m_PhysicsComponent && m_TexturePtr)
	{
		const SDL_Point Center = m_PhysicsComponent->GetCenter();
		SDL_RenderCopyEx(Engine::Get()->GetRenderer(), m_TexturePtr.get(), nullptr, &m_PhysicsComponent->GetRectTransform(), m_PhysicsComponent->GetRotationAngle(), &Center, SDL_FLIP_NONE);
	}
}

void TextureComponent::SetTextureFromAssetName(std::string Name)
{
	m_TexturePath = "Resources/Images/" + Name;
}
