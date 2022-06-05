#pragma once

#include "Precompile.h"

#include "StrokeStyle.h"
#include "TextBox.h"

namespace d14engine::ui
{
    struct AnimTextBox : TextBox, StrokeStyle
    {
        AnimTextBox(const D2D1_RECT_F& rect, float roundRadius = 0.0f);

        D2D1_COLOR_F backgroundIdleColor = { 0.95f, 0.95f, 0.95f, 1.0f };
        D2D1_COLOR_F backgroundHoverColor = { 0.92f, 0.92f, 0.92f, 1.0f };
        D2D1_COLOR_F backgroundActiveColor = { 0.98f, 0.98f, 0.98f, 1.0f };

        float staticBottomLineOffsetY = -1.0f;
        float staticBottomLineStrokeWidth = 2.0f;

        D2D1_COLOR_F staticBottomLineIdleColor = { 0.7f, 0.7f, 0.7f, 1.0f };
        D2D1_COLOR_F staticBottomLineHoverColor = { 0.5f, 0.5f, 0.5f, 1.0f };

        float bottomLineOffsetY = -1.5f;
        float bottomLineStrokeWidth = 3.0f;

        D2D1_COLOR_F bottomLineColor = (D2D1::ColorF)D2D1::ColorF::Crimson;

        float bottomLineVariableSpeedSecs = 0.2f;
        float bottomLineUniformSpeedSecs = 0.1f;

    protected:
        D2D1_COLOR_F m_staticBottomLineColor;

        float m_currBottomLineLength = 0.0f;

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererUpdateObject2DHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnGetFocusHelper() override;

        bool OnLoseFocusHelper() override;

        bool OnMouseEnterHelper(MouseEnterEvent& e) override;

        bool OnMouseLeaveHelper(MouseLeaveEvent& e) override;
    };
}