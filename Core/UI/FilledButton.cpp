#include "Precompile.h"

#include "UI/FilledButton.h"

namespace d14engine::ui
{
    FilledButton::FilledButton(
        WstrParam text,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_RECT_F& rect,
        float roundRadius)
        :
        Button(text, icon, rect, roundRadius)
    {
        solidColor = (D2D1::ColorF)D2D1::ColorF::Firebrick;
        solidColorOpaque = 1.0f;

        if (textLabel != nullptr)
        {
            textLabel->textColor = (D2D1::ColorF)D2D1::ColorF::WhiteSmoke;
        }
    }

    bool FilledButton::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        Button::OnMouseButtonHelper(e);

        if (e.status.LeftDown())
        {
            solidColor = (D2D1::ColorF)D2D1::ColorF::DarkRed;
        }
        else if (e.status.LeftUp())
        {
            solidColor = { 0.62f, 0.12f, 0.12f, 1.0f };
        }
        return false;
    }

    bool FilledButton::OnMouseEnterHelper(MouseEnterEvent& e)
    {
        Button::OnMouseEnterHelper(e);

        solidColor = { 0.62f, 0.12f, 0.12f, 1.0f };

        return false;
    }

    bool FilledButton::OnMouseLeaveHelper(MouseLeaveEvent& e)
    {
        Button::OnMouseLeaveHelper(e);

        solidColor = (D2D1::ColorF)D2D1::ColorF::Firebrick;

        return false;
    }
}