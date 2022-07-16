#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct ShadowStyle
    {
        ShadowStyle(
            UINT bitmapWidth,
            UINT bitmapHeight,
            D2D1_COLOR_F color = D2D1::ColorF{ 0x808080 },
            float standardDeviation = 3.0f,
            D2D1_SHADOW_OPTIMIZATION optimization = D2D1_SHADOW_OPTIMIZATION_BALANCED);

        D2D1_COLOR_F color = {};
        float opacity = 1.0f;

        float standardDeviation = {};

        D2D1_SHADOW_OPTIMIZATION optimization = {};

        ComPtr<ID2D1Bitmap1> bitmap = {};

        void LoadShadowBitmap(UINT width, UINT height);

        void BeginDrawOnShadow(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform = D2D1::Matrix3x2F::Identity());

        void EndDrawOnShadow(ID2D1DeviceContext* context);

        void ConfigShadowEffectInput(ID2D1Effect* effect);
    };
}