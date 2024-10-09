#pragma once
#include <iostream>
#include <list>
#include <memory>    // For std::shared_ptr and std::unique_ptr
#include <string>    // For std::string
#include <SDL_rect.h> // For SDL_Rect
#include <SDL_ttf.h>  // For TTF_Font and related functions
#include "TextWidget.h"

namespace EngineCore
{
    inline auto FontDeleter = [](TTF_Font* font)
    {
        if (font)
        {
            printf("Close Font");
            TTF_CloseFont(font);
        }
    };

    class GUI
    {
    public:
        GUI()
           
        {
            TTF_Font* Font = TTF_OpenFont(TitleFontPath.c_str(), 24);

            if (!Font)
            {
                std::cerr<<"Failed to open font " << TitleFontPath.c_str() << TTF_GetError();
                return;
            }
            m_Font = std::shared_ptr<TTF_Font>(
                Font,
                FontDeleter);
        }

        ~GUI()
        {
            printf("GUI destroyed \n");
            m_Font = nullptr;
        }

        void Draw() const;
        virtual void Update(float DeltaTime) = 0;
        virtual void UnInitialize() = 0;

        shared_ptr<TextWidget> CreateTextWidget(const string& Text, const SDL_Rect Rect);

    private:
        string TitleFontPath = "WorkSans-Regular.ttf";
        std::list<std::shared_ptr<TextWidget>> m_Widgets;
        std::shared_ptr<TTF_Font> m_Font;
    };
}
