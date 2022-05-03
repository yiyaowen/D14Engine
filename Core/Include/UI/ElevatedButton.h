#pragma once

#include "Precompile.h"

#include "FilledButton.h"
#include "ShadowStyle.h"

namespace d14engine::ui
{
    struct ElevatedButton : FilledButton, ShadowStyle
    {
        ElevatedButton(
            WstrParam text,
            ComPtrParam<ID2D1Bitmap1> icon,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f);

        // Override interface methods.

        // IDrawObject2D
        void OnRendererDrawD2D1Layer(Renderer* rndr) override;

        void OnRendererDrawD2D1Object(Renderer* rndr) override;

        // IUIObject
        void OnSizeHelper(SizeEvent& e) override;
    };
}