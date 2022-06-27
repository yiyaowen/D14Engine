#pragma once

#include "Common/Precompile.h"

#include "UIKit/FilledButton.h"
#include "UIKit/ShadowStyle.h"

namespace d14engine::uikit
{
    struct ElevatedButton : FilledButton, protected ShadowStyle
    {
        ElevatedButton(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr,
            const D2D1_COLOR_F& normalColor = { 0.98f, 0.98f, 0.98f, 1.0f },
            const D2D1_COLOR_F& activeColor = { 0.95f, 0.95f, 0.95f, 1.0f });

        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1LayerHelper(Renderer* rndr) override;

        void OnRendererDrawD2D1ObjectHelper(Renderer* rndr) override;

        // Panel
        void OnSizeHelper(SizeEvent& e) override;

        void OnChangeThemeHelper(WstrViewParam themeName) override;

        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseLeaveHelper(MouseMoveEvent& e) override;
    };
}