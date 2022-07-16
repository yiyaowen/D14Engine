#include "Common/Precompile.h"

#include "UIKit/Button.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/Application.h"

namespace d14engine::uikit
{
    Button::Button(
        WstrParam text,
        const D2D1_RECT_F& rect,
        float roundRadius,
        ComPtrParam<ID2D1Bitmap1> icon)
        :
        Panel(rect, Resu::SOLID_COLOR_BRUSH),
        ClickablePanel(rect, Resu::SOLID_COLOR_BRUSH),
        iconRect(SelfCoordRect()),
        mask(Mathu::Rounding(Width()), Mathu::Rounding(Height()))
    {
        m_takeOverChildrenDrawing = true;

        roundRadiusX = roundRadiusY = roundRadius;

        // Create text label.
        textLabel = MakeUIObject<Label>(text, SelfCoordRect());

        // Keep the text always in the center of the button.
        textLabel->f_onParentSizeAfter = [this](Panel* p, SizeEvent& e)
        {
            p->Resize(e.size.width, e.size.height);
        };
    }

    void Button::OnInitializeFinish()
    {
        ClickablePanel::OnInitializeFinish();

        textLabel->SetParent(shared_from_this());
    }

    void Button::UpdateAppearanceSetting(State state)
    {
        // TODO: add button appearance setting updating logic.
    }

    void Button::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        Panel::OnRendererDrawD2D1LayerHelper(rndr);

        // Hide children by default (Only draw self part).
        mask.BeginMaskDraw(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            // Background
            Resu::SOLID_COLOR_BRUSH->SetColor(background.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(background.opacity);

            if (brush != nullptr)
            {
                rndr->d2d1DeviceContext->FillRoundedRectangle(
                    { m_absoluteRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get());
            }
            if (bitmap != nullptr)
            {
                rndr->d2d1DeviceContext->DrawBitmap(bitmap.Get(), SelfCoordToAbsolute(iconRect), bitmapOpacity);
            }

            // Text
            if (textLabel != nullptr)
            {
                textLabel->OnRendererDrawD2D1Object(rndr);
            }

            // Outline
            Resu::SOLID_COLOR_BRUSH->SetColor(stroke.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(stroke.opacity);

            auto innerRect = Mathu::Stretch(m_absoluteRect, { -stroke.width * 0.5f, -stroke.width * 0.5f });

            rndr->d2d1DeviceContext->DrawRoundedRectangle(
                { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), stroke.width);
        }
        mask.EndMaskDraw(rndr->d2d1DeviceContext.Get());
    }

    void Button::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        rndr->d2d1DeviceContext->DrawBitmap(mask.bitmap.Get(), m_absoluteRect, mask.opacity);
    }

    bool Button::IsHit(Event::Point& p)
    {
        return Mathu::IsInside(p, m_absoluteRect);
    }

    void Button::OnSizeHelper(SizeEvent& e)
    {
        ClickablePanel::OnSizeHelper(e);

        mask.LoadMaskBitmap(Mathu::Rounding(e.size.width), Mathu::Rounding(e.size.height));
    }

    bool Button::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        UpdateAppearanceSetting(State::Hover);

        return ClickablePanel::OnMouseEnterHelper(e);
    }

    bool Button::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        UpdateAppearanceSetting(State::Idle);

        return ClickablePanel::OnMouseLeaveHelper(e);
    }

    void Button::SetEnabled(bool value)
    {
        Panel::SetEnabled(value);

        mask.opacity = value ? 1.0f : 0.45f;
    }

    void Button::OnMouseButtonPressHelper(Event& e)
    {
        if (e.Left()) UpdateAppearanceSetting(State::Down);
    }

    void Button::OnMouseButtonReleaseHelper(Event& e)
    {
        if (e.Left()) UpdateAppearanceSetting(State::Hover);
    }
}