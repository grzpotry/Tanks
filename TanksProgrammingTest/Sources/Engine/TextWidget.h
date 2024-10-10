#pragma once
#include <memory>
#include <SDL_rect.h>
#include <SDL_ttf.h>
#include <string>
#include "ResourceManager.h"

namespace EngineCore
{
    using namespace std;

    // UI widget with specific font and text
    class TextWidget
    {
    public:
        TextWidget(const string& Text, const SDL_Rect Rect, const SDL_Color& Color, weak_ptr<TTF_Font> Font)
            : m_Text(Text), m_RectTransform(Rect), m_Color(Color), m_Font(std::move(Font))
        {
            UpdateTexture();
        }

        void Draw() const;
        void Update(const string& Text);

        void UpdateTexture();
        bool IsVisible() const { return m_IsVisible; }
        void SetIsVisible(bool IsVisible);

    private:
        bool m_IsVisible = true;
        string m_Text;
        SDL_Rect m_RectTransform;
        SDL_Color m_Color;
        weak_ptr<TTF_Font> m_Font;
        shared_ptr<SDL_Texture> m_Texture;
    };
}
