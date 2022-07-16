#include "Common/Precompile.h"

#include "UIKit/AnimTextBox.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UIKit/AnimationUtils.h"
#include "UIKit/Application.h"

namespace d14engine::uikit
{
    AnimTextBox::AnimTextBox(const D2D1_RECT_F& rect, float roundRadius)
        :
        TextBox(rect, roundRadius),
        mask(Mathu::Rounding(Width()), Mathu::Rounding(Height()))
    {
        stroke.width = 1.5f;
    }

    void AnimTextBox::OnInitializeFinish()
    {
        TextBox::OnInitializeFinish();

        // Note OnChangeTheme had been called in OnInitializeFinish,
        // so we need to make sure the appearance is updated here.
        background.color = backgroundIdleColor;
        m_staticBottomLineColor = staticBottomLineIdleColor;
    }

    void AnimTextBox::OnRendererUpdateObject2DHelper(Renderer* rndr)
    {
        TextBox::OnRendererUpdateObject2DHelper(rndr);

        auto deltaSecs = (float)rndr->timer->deltaSecs;

        if (Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            float dx = Animu::AccelUniformDecelMotion(
                m_currBottomLineLength,
                deltaSecs,
                VisibleTextWidth(),
                bottomLineVariableSpeedSecs,
                bottomLineUniformSpeedSecs);

            // Use the expected displacement in the next frame directly,
            // which has already been clamped to (0,visible-text-width).
            m_currBottomLineLength = dx;
        }
    }

    void AnimTextBox::OnRendererDrawD2D1LayerHelper(Renderer* rndr)
    {
        TextBox::OnRendererDrawD2D1LayerHelper(rndr);

        // Hide children by default (Only draw self part).
        mask.BeginMaskDraw(rndr->d2d1DeviceContext.Get(), D2D1::Matrix3x2F::Translation(-m_absoluteRect.left, -m_absoluteRect.top));
        {
            TextBox::OnRendererDrawD2D1ObjectHelper(rndr);

            // Frame
            Resu::SOLID_COLOR_BRUSH->SetColor(stroke.color);
            Resu::SOLID_COLOR_BRUSH->SetOpacity(stroke.opacity);

            auto innerRect = Mathu::Stretch(m_absoluteRect, { -stroke.width * 0.5f, -stroke.width * 0.5f });

            rndr->d2d1DeviceContext->DrawRoundedRectangle(
                { innerRect, roundRadiusX, roundRadiusY }, Resu::SOLID_COLOR_BRUSH.Get(), stroke.width);

            // Bottom Line
            Resu::SOLID_COLOR_BRUSH->SetOpacity(1.0f);

            D2D1_POINT_2F left, right;

            auto visibleTextAreaLeftTop = VisibleTextAbsolutePosition();

            // Static One
            Resu::SOLID_COLOR_BRUSH->SetColor(m_staticBottomLineColor);

            left = { visibleTextAreaLeftTop.x, visibleTextAreaLeftTop.y + VisibleTextHeight() + staticBottomLineOffsetY };
            right = { left.x + VisibleTextWidth(), left.y };

            rndr->d2d1DeviceContext->DrawLine(left, right, Resu::SOLID_COLOR_BRUSH.Get(), staticBottomLineStrokeWidth);

            if (m_currBottomLineLength > 0.0f)
            {
                // Dynamic One
                Resu::SOLID_COLOR_BRUSH->SetColor(bottomLineColor);

                left = { visibleTextAreaLeftTop.x, visibleTextAreaLeftTop.y + VisibleTextHeight() + bottomLineOffsetY };
                right = { left.x + m_currBottomLineLength, left.y };

                rndr->d2d1DeviceContext->DrawLine(left, right, Resu::SOLID_COLOR_BRUSH.Get(), bottomLineStrokeWidth);
            }
        }
        mask.EndMaskDraw(rndr->d2d1DeviceContext.Get());
    }

    void AnimTextBox::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        rndr->d2d1DeviceContext->DrawBitmap(mask.bitmap.Get(), m_absoluteRect, mask.opacity);
    }

    void AnimTextBox::OnSizeHelper(SizeEvent& e)
    {
        TextBox::OnSizeHelper(e);

        mask.LoadMaskBitmap((UINT)(e.size.width + 0.5f), (UINT)(e.size.height + 0.5f));
    }

    void AnimTextBox::OnChangeThemeHelper(WstrViewParam themeName)
    {
        TextBox::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            backgroundIdleColor = D2D1::ColorF{ 0xfbfbfb };
            backgroundHoverColor = D2D1::ColorF{ 0xf5f5f5 };
            backgroundActiveColor = D2D1::ColorF{ 0xffffff };

            stroke.color = D2D1::ColorF{ 0xc2c2c2 };

            staticBottomLineIdleColor = D2D1::ColorF{ 0x8f8f8f };
            staticBottomLineHoverColor = D2D1::ColorF{ 0xa8a8a8 };

            bottomLineColor = D2D1::ColorF{ 0xd92929 };
        }
        else if (themeName == L"Dark")
        {
            backgroundIdleColor = D2D1::ColorF{ 0x262626 };
            backgroundHoverColor = D2D1::ColorF{ 0x292929 };
            backgroundActiveColor = D2D1::ColorF{ 0x1a1a1a };

            stroke.color = D2D1::ColorF{ 0x3d3d3d };

            staticBottomLineIdleColor = D2D1::ColorF{ 0x5c5c5c };
            staticBottomLineHoverColor = D2D1::ColorF{ 0x4c4c4c };

            bottomLineColor = D2D1::ColorF{ 0x2e8b57 };
        }

        if (Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            background.color = backgroundActiveColor;
            m_staticBottomLineColor = staticBottomLineHoverColor;
        }
        else // Simply change to idle appearance otherwise.
        {
            background.color = backgroundIdleColor;
            m_staticBottomLineColor = staticBottomLineIdleColor;
        }
    }

    bool AnimTextBox::OnGetFocusHelper()
    {
        background.color = backgroundActiveColor;

        Application::APP->IncreaseAnimateCount();

        return TextBox::OnGetFocusHelper();
    }

    bool AnimTextBox::OnLoseFocusHelper()
    {
        background.color = backgroundIdleColor;
        m_staticBottomLineColor = staticBottomLineIdleColor;

        m_currBottomLineLength = 0.0f;

        Application::APP->DecreaseAnimateCount();

        return TextBox::OnLoseFocusHelper();
    }

    bool AnimTextBox::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        if (!Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            background.color = backgroundHoverColor;
            m_staticBottomLineColor = staticBottomLineHoverColor;
        }
        return Panel::OnMouseEnterHelper(e);
    }

    bool AnimTextBox::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        if (!Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            background.color = backgroundIdleColor;
            m_staticBottomLineColor = staticBottomLineIdleColor;
        }
        return Panel::OnMouseLeaveHelper(e);
    }

    void AnimTextBox::SetEnabled(bool value)
    {
        TextBox::SetEnabled(value);

        mask.opacity = value ? 1.0f : 0.45f;

        background.color = backgroundIdleColor;
        m_staticBottomLineColor = staticBottomLineIdleColor;
    }
}