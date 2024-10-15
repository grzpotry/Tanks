#include "GUI.h"
#include <iostream>
#include "Engine.h"

namespace EngineCore
{
    class TextWidget;
    
    void GUI::Draw() const
    {
        for (const auto& Widget : m_Widgets)
        {
            if (Widget->IsVisible())
            {
                Widget->Draw();
            }
        }
    }

    std::shared_ptr<TextWidget> GUI::CreateTextWidget(const string& Text, const SDL_Rect Rect, std::vector<shared_ptr<TextWidget>>* ParentPanel, FontSize Font, SDL_Color Color)
    {
        weak_ptr font = m_SmallFont;

        switch (Font)
        {
            case Small:
                break;
            case Medium:
                font = m_MediumFont;
            case Large:
                font = m_LargeFont;
                break;
        }
        
        const auto Widget = make_shared<TextWidget>(Text, Rect, Color, font);
        m_Widgets.push_back(Widget);

        if (ParentPanel)
        {
            ParentPanel->push_back(Widget);
        }

        return Widget;
    }

    SDL_Rect GUI::GetLayoutRect(WidgetAnchor Anchor, int Width, int Height)
    {
        auto WindowSize = Engine::Get()->GetWindowSize();
        const float HorizontalMargin = 0.05f * WindowSize.Y;
        const float VerticalMargin = 0.05f * WindowSize.X;
        
        SDL_Rect c;
        c.w = Width;
        c.h = Height;

        switch (Anchor)
        {
            case BottomLeft:
                c.x = VerticalMargin;
                c.y = WindowSize.Y - Height - HorizontalMargin;
                break;
            case MiddleCenter:
                c.x = WindowSize.X / 2 - Width / 2;
                c.y = WindowSize.Y / 2 - Height / 2;
                break;
            case TopCenter:
                c.x = WindowSize.X / 2 - Width / 2;
                c.y = VerticalMargin;
                break;
            case BottomCenter:
                c.x = WindowSize.X / 2 - Width / 2;
                c.y = WindowSize.Y - Height - HorizontalMargin;
                break;
        }

        return c;
    }

    void GUI::RemoveWidget(const shared_ptr<TextWidget>& Widget)
    {
        m_Widgets.remove(Widget);
    }

    shared_ptr<TTF_Font> GUI::LoadFont(int Size) const
    {
        TTF_Font* Font = TTF_OpenFont(TitleFontPath.c_str(), Size);

        if (!Font)
        {
            std::cerr << "Failed to open font " << TitleFontPath.c_str() << TTF_GetError();
            return nullptr;
        }
        return {
            Font,
            FontDeleter
        };
    }
}

