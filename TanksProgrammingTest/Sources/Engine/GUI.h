#pragma once
#include <list>
#include <memory>
#include <string>
#include <SDL_rect.h>
#include <SDL_ttf.h>
#include "TextWidget.h"

namespace EngineCore
{
    inline auto FontDeleter = [](TTF_Font* font)
    {
        if (font)
        {
            TTF_CloseFont(font);
        }
    };

    enum FontSize :uint8_t
    {
        Small = 1 << 0,
        Medium = 1 << 1
    };

    enum WidgetAnchor :uint8_t
    {
        BottomLeft = 1 << 0,
        MiddleCenter = 1 << 1,
        BottomCenter = 1 << 2
    };

    // Graphical User Interface, renders widgets
    class GUI
    {
    public:
        GUI()
        {
            m_SmallFont = LoadFont(32);
            m_MediumFont = LoadFont(48);
        }

        virtual ~GUI()
        {
            printf("GUI destroyed \n");
        }

        static SDL_Rect GetLayoutRect(WidgetAnchor Anchor, int Width, int Height);

        void Draw() const;
        virtual void Update(float DeltaTime) = 0;
        virtual void UnInitialize() = 0;
        
        void RemoveWidget(const shared_ptr<TextWidget>& Widget);
        shared_ptr<TextWidget> CreateTextWidget(const string& Text, const SDL_Rect Rect, FontSize Font = Small);
        
    private:
        const string TitleFontPath = "Resources/Fonts/ARCADE.ttf";
        
        std::list<std::shared_ptr<TextWidget>> m_Widgets;
        std::shared_ptr<TTF_Font> m_SmallFont;
        std::shared_ptr<TTF_Font> m_MediumFont;

        shared_ptr<TTF_Font> LoadFont(int Size) const;
    };
}
