#pragma once

#include "Common/Precompile.h"

#include "UIKit/FlatButton.h"

namespace d14engine::uikit
{
    struct FilledButton : FlatButton
    {
        FilledButton(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr);

    protected:
        // Override interface methods.

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;
    };
}