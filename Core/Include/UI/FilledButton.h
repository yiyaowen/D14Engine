#pragma once

#include "Precompile.h"

#include "Button.h"
#include "Label.h"

namespace d14engine::ui
{
    struct FilledButton : Button
    {
        FilledButton(
            WstrParam text,
            ComPtrParam<ID2D1Bitmap1> icon,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f);

        // Override interface methods.

        // IUIObject
        bool OnMouseButtonHelper(MouseButtonEvent& e) override;

        bool OnMouseEnterHelper(MouseEnterEvent& e) override;

        bool OnMouseLeaveHelper(MouseLeaveEvent& e) override;
    };
}