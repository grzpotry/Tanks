#include "PlayerInputComponent.h"
#include "TextureComponent.h"
#include "Entity.h"
#include "Engine.h"

PlayerInputComponent::PlayerInputComponent(Entity* Owner)
	: EntityComponent(Owner)
	, m_TextureComponent(nullptr)
{
}

PlayerInputComponent::PlayerInputComponent()
	: PlayerInputComponent(nullptr)
{
}

void PlayerInputComponent::Initialize()
{
	m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();
}

void PlayerInputComponent::Update(float DeltaTime)
{
	constexpr int Speed = 150;
	const int MoveDistance = Speed * DeltaTime;

	if (MoveDistance == 0)
	{
		printf("Can't move player, very small delta time [%f].  ", DeltaTime);
	}
	
	SDL_Rect& Rectangle = m_TextureComponent->GetRectangle();
	std::vector<SDL_Event> Events = Engine::Get()->GetEvents();
	const Uint8* Keystates = SDL_GetKeyboardState(NULL);

	//TODO: add some input handling layer
	if (Keystates[SDL_SCANCODE_LEFT] || Keystates[SDL_SCANCODE_A]) {
		Rectangle.x -= MoveDistance;
		m_TextureComponent->SetRotationAngle(-90);
	}
	else if (Keystates[SDL_SCANCODE_RIGHT] || Keystates[SDL_SCANCODE_D]) {
		Rectangle.x += MoveDistance;
		m_TextureComponent->SetRotationAngle(90);
	}
	else if (Keystates[SDL_SCANCODE_W] || Keystates[SDL_SCANCODE_UP]) {
		Rectangle.y -= MoveDistance;
		m_TextureComponent->SetRotationAngle(0);
	}
	else if (Keystates[SDL_SCANCODE_S] || Keystates[SDL_SCANCODE_DOWN]) {
		Rectangle.y += MoveDistance;
		m_TextureComponent->SetRotationAngle(180);
	}
	
	// for (const SDL_Event& Event : Events)
	// {
	// 	switch (Event.type) 
	// 	{
	// 		case SDL_KEYDOWN:
	// 		{
	// 			switch (Event.key.keysym.scancode)
	// 			{
	// 				case SDL_SCANCODE_W :
	// 				case SDL_SCANCODE_UP :
	// 					Rectangle.y -= Speed / 30;
	// 					break;
	// 				case SDL_SCANCODE_A :
	// 				case SDL_SCANCODE_LEFT :
	// 					Rectangle.x -= Speed / 30;
	// 					break;
	// 				case SDL_SCANCODE_S :
	// 				case SDL_SCANCODE_DOWN :
	// 					Rectangle.y += Speed / 30;
	// 					break;
	// 				case SDL_SCANCODE_D :
	// 				case SDL_SCANCODE_RIGHT :
	// 					Rectangle.x += Speed / 30;
	// 					break;
	// 				default:
	// 					break;
	// 			}
	// 		}
	// 	}
	// }

	int MaxWidth = 0, MaxHeight = 0;
	SDL_GetWindowSize(Engine::Get()->GetWindow(), &MaxWidth, &MaxHeight);

	//restrict movement
	if (Rectangle.x + Rectangle.w > MaxWidth)
	{
		Rectangle.x = MaxWidth - Rectangle.w;
	}

	if (Rectangle.x < 0)
	{
		Rectangle.x = 0;
	}

	if (Rectangle.y + Rectangle.h > MaxHeight)
	{
		Rectangle.y = MaxHeight - Rectangle.h;
	}

	if (Rectangle.y < 0)
	{
		Rectangle.y = 0;
	}
}
