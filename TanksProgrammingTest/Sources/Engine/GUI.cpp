#include "GUI.h"

namespace EngineCore
{
    class TextWidget;
    
    void GUI::Draw() const
    {
        for (const auto& Widget : m_Widgets)
        {
            Widget->Draw();
        }
    }

    std::shared_ptr<TextWidget> GUI::CreateTextWidget(const string& Text, const SDL_Rect Rect)
    {
        SDL_Color Color = {0, 255, 0, 255};
        const auto Widget = make_shared<TextWidget>(Text, Rect, Color, m_Font);
        
        m_Widgets.push_back(Widget);

        return Widget;
    }
}

