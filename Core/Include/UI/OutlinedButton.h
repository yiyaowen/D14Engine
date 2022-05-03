#pragma once

#include "Precompile.h"

#include "FlatButton.h"
#include "Label.h"

namespace d14engine::ui
{
    struct OutlinedButton : FlatButton
    {
        OutlinedButton(
            WstrParam text,
            ComPtrParam<ID2D1Bitmap1> icon,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            float strokeWidth = 2.0f);

        float strokeWidth;

        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Object(Renderer* rndr) override;
    };
}