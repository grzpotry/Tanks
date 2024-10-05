#include "TextureComponent.h"
#include <filesystem>
#include "Engine.h"
#include "Entity.h"

class PhysicsComponent;

TextureComponent::TextureComponent(Entity* Owner)
	: EntityComponent(Owner)
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
	m_PhysicsComponent = GetOwner()->GetComponentWeak<PhysicsComponent>();
	LoadTexture(m_TexturePath, m_TexturePtr);
}

void TextureComponent::UnInitialize()
{
	m_TexturePtr = nullptr;
}

void TextureComponent::Draw()
{
	if (const auto sharedComponent =m_PhysicsComponent.lock() )
	{
		if (m_TexturePtr)
		{
			const SDL_Point Center = sharedComponent->GetCenter();
			SDL_RenderCopyEx(Engine::Get()->GetRenderer(), m_TexturePtr.get(), nullptr, &sharedComponent->GetRectTransform(), sharedComponent->GetRotationAngle(), &Center, SDL_FLIP_NONE);
		}
	}
}

void TextureComponent::SetTextureFromAssetName(std::string Name)
{
	m_TexturePath = "Resources/Images/" + Name;
}
