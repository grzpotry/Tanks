#include "Engine.h"
#include "TextWidget.h"

namespace EngineCore
{
    void TextWidget::Draw() const
    {
        if (m_Texture)
        {
            SDL_RenderCopy(Engine::Get()->GetRenderer(), m_Texture.get(), nullptr, &m_RectTransform);
        }
    }

    void TextWidget::Update(const string& Text)
    {
        if (m_Text == Text)
        {
            return;
        }
        
        m_Text = Text;
        UpdateTexture();
    }

    void TextWidget::UpdateTexture()
    {
        if (m_Texture != nullptr)
        {
            m_Texture.reset();
        }
            
        if (auto Font = m_Font.lock())
        {
            SDL_Surface* Surface = TTF_RenderText_Blended (Font.get(), m_Text.c_str(), m_Color);
            
            if (!Surface)
            {
                SDL_Log("Failed to create text surface: %s", TTF_GetError());
                return;
            }

            m_Texture = ResourceManager::CreateTexture(Surface);
            SDL_FreeSurface(Surface);
        }
    }

    void TextWidget::SetIsVisible(bool IsVisible)
    {
        m_IsVisible = IsVisible;
    }
}
