#pragma once

#include "Common/Precompile.h"

#include "UIKit/MaskStyle.h"
#include "UIKit/StrokeStyle.h"
#include "UIKit/TextBox.h"

namespace d14engine::uikit
{
    struct AnimTextBox : TextBox
    {
        AnimTextBox(const D2D1_RECT_F& rect, float roundRadius = 0.0f);

        void OnInitializeFinish() override;

        MaskStyle mask = { 0, 0 };
        StrokeStyle stroke = {};

        D2D1_COLOR_F backgroundIdleColor = {};
        D2D1_COLOR_F backgroundHoverColor = {};
        D2D1_COLOR_F backgroundActiveColor = {};

        float staticBottomLineOffsetY = -1.0f;
        float staticBottomLineStrokeWidth = 2.0f;

        D2D1_COLOR_F staticBottomLineIdleColor = {};
        D2D1_COLOR_F staticBottomLineHoverColor = {};

        float bottomLineOffsetY = -1.0f;
        float bottomLineStrokeWidth = 2.0f;

        D2D1_COLOR_F bottomLineColor = {};

        float bottomLineVariableSpeedSecs = 0.2f;
        float bottomLineUniformSpeedSecs = 0.1f;

    protected:
        D2D1_COLOR_F m_staticBottomLineColor = {};

        float m_currBottomLineLength = 0.0f;

    protected:
        // Override interface methods.

        // IDrawObject2D
        void OnRendererUpdateObject2DHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnGetFocusHelper() override;

        bool OnLoseFocusHelper() override;

        bool OnMouseEnterHelper(MouseMoveEvent& e) override;

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;

    public:
        void SetEnabled(bool value) override;
    };
}