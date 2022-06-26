#include "Precompile.h"

#include "UI/AnimTextBox.h"

#include "Renderer/MathUtils.h"
using namespace d14engine::renderer;

#include "UI/AnimationUtils.h"
#include "UI/Application.h"

namespace d14engine::ui
{
    AnimTextBox::AnimTextBox(const D2D1_RECT_F& rect, float roundRadius)
        :
        TextBox(rect, roundRadius),
        StrokeStyle(1.5f, { 0.8f, 0.8f, 0.8f, 1.0f }),
        m_staticBottomLineColor(staticBottomLineIdleColor)
    {
        backgroundColor = { 0.95f, 0.95f, 0.95f, 1.0f };
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

    void AnimTextBox::OnRendererDrawD2D1ObjectHelper(Renderer* rndr)
    {
        TextBox::OnRendererDrawD2D1ObjectHelper(rndr);

        // Frame
        UIResu::SOLID_COLOR_BRUSH->SetColor(strokeColor);
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(strokeOpacity);

        auto innerRect = Mathu::Stretch(m_absoluteRect, { -strokeWidth * 0.5f, -strokeWidth * 0.5f });

        rndr->d2d1DeviceContext->DrawRoundedRectangle(
            { innerRect, roundRadiusX, roundRadiusY }, UIResu::SOLID_COLOR_BRUSH.Get(), strokeWidth);

        // Bottom Line
        UIResu::SOLID_COLOR_BRUSH->SetOpacity(1.0f);

        D2D1_POINT_2F left, right;

        auto visibleTextAreaLeftTop = VisibleTextAbsolutePosition();

        // Static One
        UIResu::SOLID_COLOR_BRUSH->SetColor(m_staticBottomLineColor);

        left = { visibleTextAreaLeftTop.x, visibleTextAreaLeftTop.y + VisibleTextHeight() + staticBottomLineOffsetY };
        right = { left.x + VisibleTextWidth(), left.y };

        rndr->d2d1DeviceContext->DrawLine(left, right, UIResu::SOLID_COLOR_BRUSH.Get(), staticBottomLineStrokeWidth);

        if (m_currBottomLineLength > 0.0f)
        {
            // Dynamic One
            UIResu::SOLID_COLOR_BRUSH->SetColor(bottomLineColor);

            left = { visibleTextAreaLeftTop.x, visibleTextAreaLeftTop.y + VisibleTextHeight() + bottomLineOffsetY };
            right = { left.x + m_currBottomLineLength, left.y };

            rndr->d2d1DeviceContext->DrawLine(left, right, UIResu::SOLID_COLOR_BRUSH.Get(), bottomLineStrokeWidth);
        }
    }

    void AnimTextBox::OnChangeThemeHelper(WstrViewParam themeName)
    {
        TextBox::OnChangeThemeHelper(themeName);

        if (themeName == L"Light")
        {
            backgroundIdleColor = { 0.95f, 0.95f, 0.95f, 1.0f };
            backgroundHoverColor = { 0.92f, 0.92f, 0.92f, 1.0f };
            backgroundActiveColor = { 0.98f, 0.98f, 0.98f, 1.0f };

            strokeColor = { 0.8f, 0.8f, 0.8f, 1.0f };

            staticBottomLineIdleColor = { 0.7f, 0.7f, 0.7f, 1.0f };
            staticBottomLineHoverColor = { 0.5f, 0.5f, 0.5f, 1.0f };

            bottomLineColor = (D2D1::ColorF)D2D1::ColorF::Crimson;
        }
        else if (themeName == L"Dark")
        {
            backgroundIdleColor = { 0.17f, 0.17f, 0.17f, 1.0f };
            backgroundHoverColor = { 0.2f, 0.2f, 0.2f, 1.0f };
            backgroundActiveColor = { 0.1f, 0.1f, 0.1f, 1.0f };

            strokeColor = { 0.25f, 0.25f, 0.25f, 1.0f };

            staticBottomLineIdleColor = { 0.35f, 0.35f, 0.35f, 1.0f };
            staticBottomLineHoverColor = { 0.4f, 0.4f, 0.4f, 1.0f };

            bottomLineColor = (D2D1::ColorF)D2D1::ColorF::SeaGreen;
        }

        if (Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            backgroundColor = backgroundActiveColor;
            m_staticBottomLineColor = staticBottomLineHoverColor;
        }
        else // Simply change to idle appearance otherwise.
        {
            backgroundColor = backgroundIdleColor;
            m_staticBottomLineColor = staticBottomLineIdleColor;
        }
    }

    bool AnimTextBox::OnGetFocusHelper()
    {
        backgroundColor = backgroundActiveColor;

        Application::APP->IncreaseAnimateCount();

        return TextBox::OnGetFocusHelper();
    }

    bool AnimTextBox::OnLoseFocusHelper()
    {
        backgroundColor = backgroundIdleColor;
        m_staticBottomLineColor = staticBottomLineIdleColor;

        m_currBottomLineLength = 0.0f;

        Application::APP->DecreaseAnimateCount();

        return TextBox::OnLoseFocusHelper();
    }

    bool AnimTextBox::OnMouseEnterHelper(MouseMoveEvent& e)
    {
        if (!Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            backgroundColor = backgroundHoverColor;
            m_staticBottomLineColor = staticBottomLineHoverColor;
        }
        return Panel::OnMouseEnterHelper(e);
    }

    bool AnimTextBox::OnMouseLeaveHelper(MouseMoveEvent& e)
    {
        if (!Application::APP->IsUIObjectFocused(weak_from_this()))
        {
            backgroundColor = backgroundIdleColor;
            m_staticBottomLineColor = staticBottomLineIdleColor;
        }
        return Panel::OnMouseLeaveHelper(e);
    }
}