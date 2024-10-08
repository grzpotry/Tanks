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
}
