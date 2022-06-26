#pragma once

#include "Precompile.h"

namespace d14engine::ui
{
    struct ShadowStyle
    {
        ShadowStyle(
            UINT shadowBitmapWidth,
            UINT shadowBitmapHeight,
            float shadowStandardDeviation = 3.0f,
            D2D1_COLOR_F shadowColor = (D2D1::ColorF)D2D1::ColorF::Gray,
            D2D1_SHADOW_OPTIMIZATION shadowOptimization = D2D1_SHADOW_OPTIMIZATION_BALANCED);

        float shadowStandardDeviation = {};

        D2D1_COLOR_F shadowColor = {};
        float shadowOpacity = 1.0f;

        D2D1_SHADOW_OPTIMIZATION shadowOptimization = {};

        ComPtr<ID2D1Bitmap1> shadowBitmap = {};

        void LoadShadowBitmap(UINT width, UINT height);

        void BeginDrawOnShadow(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform = D2D1::Matrix3x2F::Identity());

        void EndDrawOnShadow(ID2D1DeviceContext* context);

        void ConfigShadowEffectInput(ID2D1Effect* effect);
    };
}