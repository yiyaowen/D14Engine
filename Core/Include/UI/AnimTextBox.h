#pragma once

#include "Precompile.h"

#include "StrokeStyle.h"
#include "TextBox.h"

namespace d14engine::ui
{
    struct AnimTextBox : TextBox, StrokeStyle
    {
        AnimTextBox(const D2D1_RECT_F& rect, float roundRadius = 0.0f);

        float staticBottomLineOffsetY = -1.0f;
        float staticBottomLineStrokeWidth = 2.0f;

        D2D1_COLOR_F staticBottomLineColor = { 0.7f, 0.7f, 0.7f, 1.0f };

        float bottomLineOffsetY = -1.5f;
        float bottomLineStrokeWidth = 3.0f;

        D2D1_COLOR_F bottomLineColor = (D2D1::ColorF)D2D1::ColorF::Crimson;

        float bottomLineVariableSpeedSecs = 0.2f;
        float bottomLineUniformSpeedSecs = 0.1f;

    protected:
        float m_currBottomLineLength = 0.0f;

    public:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererUpdateObject2D(Renderer* rndr) override;

        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // IUIObject
        bool OnGetFocusHelper() override;

        bool OnLoseFocusHelper() override;

        bool OnMouseEnterHelper(MouseEnterEvent& e) override;

        bool OnMouseLeaveHelper(MouseLeaveEvent& e) override;
    };
}