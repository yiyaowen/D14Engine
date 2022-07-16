#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct MaskStyle
    {
        MaskStyle(
            UINT bitmapWidth,
            UINT bitmapHeight,
            D2D1_COLOR_F color = D2D1::ColorF{ 0x000000, 0.0f });

        D2D1_COLOR_F color = {};
        float opacity = 1.0f;

        ComPtr<ID2D1Bitmap1> bitmap = {};

        void LoadMaskBitmap(UINT width, UINT height);

        void BeginMaskDraw(ID2D1DeviceContext* context, const D2D1_MATRIX_3X2_F& transform = D2D1::Matrix3x2F::Identity());

        void EndMaskDraw(ID2D1DeviceContext* context);
    };
}