#pragma once

#include "Precompile.h"

#include "Button.h"
#include "Label.h"
#include "StrokeStyle.h"

namespace d14engine::ui
{
    struct OutlinedButton : Button
    {
        OutlinedButton(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr,
            const D2D1_COLOR_F& strokeColor = { 0.78f, 0.12f, 0.2f, 1.0f },
            float strokeWidth = 1.5f);
    };
}