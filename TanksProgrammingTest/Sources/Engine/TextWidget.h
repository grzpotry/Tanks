#pragma once
#include <memory>
#include <SDL_rect.h>
#include <SDL_ttf.h>
#include <string>
#include "ResourceManager.h"

namespace EngineCore
{
    using namespace std;

    class TextWidget
    {
    public:
        TextWidget(const string& Text, const SDL_Rect Rect, const SDL_Color& Color, weak_ptr<TTF_Font> Font)
            : m_Text(Text), m_RectTransform(Rect), m_Color(Color), m_Font(std::move(Font))
        {
            UpdateTexture();
        }

        ~TextWidget()
        {
            printf("Destroyed text widget %s \n", m_Text.c_str());
            m_Font.reset();
            m_Texture = nullptr;
        }

        void Draw() const;
        void Update(const string& Text);

        void UpdateTexture()
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

    private:
        string m_Text = "";
        SDL_Rect m_RectTransform;
        SDL_Color m_Color;
        weak_ptr<TTF_Font> m_Font;
        shared_ptr<SDL_Texture> m_Texture;
    };
}
