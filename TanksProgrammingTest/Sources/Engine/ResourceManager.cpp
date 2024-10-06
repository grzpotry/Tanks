#include "ResourceManager.h"
#include "EngineUtils.h"
#include "Entity.h"
#include "Components/EntityComponent.h"
#include <filesystem>
#include <fstream>
#include <SDL_image.h>
#include <SDL_render.h>

#include "Engine.h"

namespace Engine
{
	ResourceManager::ResourceManager(std::string Path)
		: m_Path(Path)
	{
	}

	void ResourceManager::LoadResources()
	{
		LoadResourcesFromFolder("Entities", m_Entities);
		LoadResourcesFromFolder("Scenes", m_Scenes);
	}

	const nlohmann::json& ResourceManager::GetJsonConfig(string Name, enum ResourceType Type)
	{
		switch (Type)
		{
		case ResourceType::Entity:
			return m_Entities[Name];
		case ResourceType::Scene:
			return m_Scenes[Name];
		}

		static nlohmann::json DefaultJsonConfig;
		return DefaultJsonConfig;
	}

	void ResourceManager::RegisterComponent(string Type, EntityComponent* Component)
	{
		m_ComponentsPrototypes[Type] = Component;
	}

	const EntityComponent* ResourceManager::GetComponentPrototypeByName(string Name)
	{
		map<string, EntityComponent*>::iterator ComponentPrototypeIt = m_ComponentsPrototypes.find(Name);
		if (ComponentPrototypeIt != m_ComponentsPrototypes.end())
		{
			return ComponentPrototypeIt->second;
		}

		return nullptr;
	}

	unique_ptr<Entity> ResourceManager::CreateEntityFromDataTemplate(string Name)
	{
		map<string, nlohmann::json>::iterator EntityDataTemplateIt = m_Entities.find(Name);
		if (EntityDataTemplateIt != m_Entities.end())
		{
			auto NewEntity = make_unique<Entity>();
			NewEntity->LoadFromConfig(EntityDataTemplateIt->second);
			return NewEntity;
		}

		return nullptr;
	}

	void ResourceManager::LoadResourcesFromFolder(string Folder, map<string, nlohmann::json>& MapContainer) const
	{
		string FolderPath = m_Path + "/" + Folder;
		for (const auto& Entry : filesystem::directory_iterator(FolderPath))
		{
			ifstream InFile(Entry.path());
			nlohmann::json JsonFile;
			InFile >> JsonFile;

			if (!JsonFile.is_null())
			{
				wstring WStrFileName = Entry.path().stem();
				string FileName = EngineUtils::WstringToString(WStrFileName);
				MapContainer.insert({ FileName, JsonFile });
			}

			InFile.close();
		}
	}


	std::shared_ptr<SDL_Texture> ResourceManager::LoadTexture(string Path)
	{
		printf("Load new texture %s \n", Path.c_str());
		SDL_Surface* Surface = IMG_Load(Path.c_str());

		if (!Surface)
		{
			printf("Couldn't load texture from path %s \n", Path.c_str());
		}

		auto TexturePtr = std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(Engine::Get()->GetRenderer(), Surface),
		                             [](SDL_Texture* Texture)
		                             {
			                             if (Texture)
			                             {
				                             SDL_DestroyTexture(Texture);
				                             printf("Destroy texture \n");
			                             }
		                             });
		
		SDL_FreeSurface(Surface);
		return TexturePtr;
	}

	void ResourceManager::ReleaseTexture(string Path)
	{
		const auto& b = m_Textures.at(Path);
		
		if (b.use_count() == 1)
		{
			m_Textures.erase(Path);
			printf("Released texture %s \n", Path.c_str());
		}
	}

	std::shared_ptr<SDL_Texture> ResourceManager::GetOrLoadTexture(string Path)
	{
		auto [it, inserted] = m_Textures.try_emplace(Path, nullptr);
		
		if (inserted)
		{
			it->second = LoadTexture(Path);
		}

		return it->second;
	}
}
