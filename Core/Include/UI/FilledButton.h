#pragma once

#include "Precompile.h"

#include "Button.h"

namespace d14engine::ui
{
    struct FilledButton : Button
    {
        FilledButton(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr,
            const D2D1_COLOR_F& idleColor = { 0.98f, 0.98f, 0.98f, 1.0f },
            const D2D1_COLOR_F& hoverColor = { 0.95f, 0.95f, 0.95f, 1.0f },
            const D2D1_COLOR_F& downColor = { 0.95f, 0.95f, 0.95f, 1.0f });

        // Override interface methods.

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;
    };
}