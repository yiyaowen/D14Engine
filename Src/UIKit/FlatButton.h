#pragma once

#include "Common/Precompile.h"

#include "UIKit/Button.h"

namespace d14engine::uikit
{
    struct FlatButton : Button
    {
        FlatButton(
            WstrParam text,
            const D2D1_RECT_F& rect,
            float roundRadius = 0.0f,
            ComPtrParam<ID2D1Bitmap1> icon = nullptr);

        void OnInitializeFinish() override;

    public:
        // The appearance settings will be updated dynamically in each render pass.

        struct Appearance
        {
            ComPtr<ID2D1Bitmap1> bitmap = {};
            float bitmapOpacity = {};

            SolidStyle foreground = {};
            SolidStyle background = {};

            StrokeStyle stroke = {};
        }
        appearances[(size_t)State::Count] = {};

    protected:
        // Override interface methods.

        // Panel
        void OnChangeThemeHelper(WstrViewParam themeName) override;

        // Button
        void UpdateAppearanceSetting(State state) override;
    };
}