#include "Precompile.h"

#include "UI/Button.h"

#include "UI/Application.h"

namespace d14engine::ui
{
    Button::Button(
        WstrParam text,
        ComPtrParam<ID2D1Bitmap1> icon,
        const D2D1_RECT_F& rect,
        float roundRadius)
        :
        Panel(rect, UIResu::SOLID_COLOR_BRUSH, icon),
        SolidColorStyle((D2D1::ColorF)D2D1::ColorF::Gray, 0.0f),
        iconRect(SelfCoordRect())
    {
        m_radiusX = m_radiusY = roundRadius;

        // Create text label. Only icon is displayed when text is empty.
        if (!text.empty())
        {
            textLabel = std::make_shared<Label>(text, SelfCoordRect());

            // Keep the text always in the center of the button.
            textLabel->f_onParentSizeAfter = [this](Panel* p, SizeEvent& e)
            {
                p->Resize(e.size.width, e.size.height);
            };
        }
    }

    void Button::OnInitializeFinish()
    {
        textLabel->SetParent(shared_from_this());
    }

    void Button::OnRendererDrawD2D1Object(Renderer* rndr)
    {
        // Background
        UIResu::SOLID_COLOR_BRUSH->SetColor(solidColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(solidColorOpaque);

        if (brush != nullptr)
        {
            D2D1_ROUNDED_RECT roundedRect = { m_absoluteRect, m_radiusX, m_radiusY };
            rndr->d2d1DeviceContext->FillRoundedRectangle(roundedRect, brush.Get());
        }
        if (bitmap != nullptr)
        {
            rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), SelfCoordToAbsolute(iconRect));
        }

        // Text
        if (textLabel != nullptr)
        {
            textLabel->OnRendererDrawD2D1Object(rndr);
        }
    }

    void Button::OnSizeHelper(SizeEvent& e)
    {
        textLabel->OnParentSize(e);
    }

    bool Button::OnMouseButtonHelper(MouseButtonEvent& e)
    {
        Panel::OnMouseButtonHelper(e);

        if (e.status.LeftDown()) m_hasLeftPressed = true;
        else if (e.status.LeftUp()) m_hasLeftPressed = false;

        if (e.status.RightDown()) m_hasRightPressed = true;
        else if (e.status.RightUp()) m_hasRightPressed = false;

        if (e.status.MiddleDown()) m_hasMiddlePressed = true;
        else if (e.status.MiddleUp()) m_hasMiddlePressed = false;

        return false;
    }

    bool Button::OnMouseLeaveHelper(MouseLeaveEvent& e)
    {
        Panel::OnMouseLeaveHelper(e);

        m_hasLeftPressed = m_hasRightPressed = m_hasMiddlePressed = false;

        return false;
    }
}