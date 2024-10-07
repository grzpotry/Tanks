#include "TextureComponent.h"
#include <filesystem>
#include "Engine.h"
#include "Entity.h"
#include "ResourceManager.h"

namespace Engine
{
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
		const string TextureName = Config.value("Texture", "");
	
		if (!TextureName.empty())
		{
			SetTextureFromAssetName(TextureName);
		}
	}

	void TextureComponent::Initialize(Scene* const Scene)
	{
		EntityComponent::Initialize(Scene);
		m_PhysicsComponentWeak = GetOwner()->GetComponentWeak<PhysicsComponent>();
		m_TexturePtr = Engine::Get()->GetResourceManager()->GetOrLoadTexture(m_TexturePath);
	}

	void TextureComponent::UnInitialize()
	{
		m_TexturePtr = nullptr;
		Engine::Get()->GetResourceManager()->ReleaseTexture(m_TexturePath);
	}

	void TextureComponent::Draw()
	{
		if (const auto Physics = m_PhysicsComponentWeak.lock())
		{
			if (m_TexturePtr)
			{
				const SDL_Point Center = Physics->GetCenter();
				SDL_RenderCopyEx(Engine::Get()->GetRenderer(), m_TexturePtr.get(), nullptr,&Physics->GetRectTransform(), Physics->GetRotationAngle(), &Center,SDL_FLIP_NONE);
			}
		}
	}

	void TextureComponent::SetTextureFromAssetName(string Name)
	{
		m_TexturePath = "Resources/Images/" + Name;
	}
}
