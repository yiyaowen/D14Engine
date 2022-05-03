#include "Precompile.h"

#include "UI/OutlinedButton.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    OutlinedButton::OutlinedButton(
        WstrParam text,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_RECT_F& rect,
        float roundRadius,
        float strokeWidth)
        :
        FlatButton(text, icon, rect, roundRadius),
        strokeWidth(strokeWidth)
    {
        if (textLabel != nullptr)
        {
            textLabel->textColor = (D2D1::ColorF)D2D1::ColorF::Firebrick;
        }
    }

    void OutlinedButton::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // Background & Text
        FlatButton::OnRendererDrawD2D1Object(rndr);

        // Outline
        UIResu::SOLID_COLOR_BRUSH->SetColor((D2D1::ColorF)D2D1::ColorF::Firebrick);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(1.0f);

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { m_absoluteRect, m_radiusX, m_radiusY }, UIResu::SOLID_COLOR_BRUSH.Get(), strokeWidth);
    }
}