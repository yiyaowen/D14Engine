#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct MaskStyle
    {
        MaskStyle(
            UINT maskBitmapWidth,
            UINT maskBitmapHeight,
            D2D1_COLOR_F maskColor = { 0.0f, 0.0f, 0.0f, 0.0f });

        D2D1_COLOR_F maskColor = {};
        float maskOpacity = 1.0f;

        ComPtr<ID2D1Bitmap1> maskBitmap = {};

        void LoadMaskBitmap(UINT width, UINT height);

        void BeginDrawOnMask(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform = D2D1::Matrix3x2F::Identity());

        void EndDrawOnMask(ID2D1DeviceContext* context);
    };
}